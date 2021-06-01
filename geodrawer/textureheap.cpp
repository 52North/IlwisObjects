/*IlwisObjects is a framework for analysis, processing and visualization of remote sensing and gis data
Copyright (C) 2018  52n North

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include "kernel.h"
#include "ilwisdata.h"
#include "coveragedisplay/coveragelayermodel.h"
#include "textureheap.h"
#include "texture.h"
#include "cctexture.h"
#include "rasterlayermodel.h"

using namespace Ilwis;
using namespace Ui;

TextureHeap::TextureHeap(RasterLayerModel * rasterLayerModel, const std::vector<IRasterCoverage> * ccBands)
: textureThread(0)
, fAbortTexGen(false)
, fStopThread(false)
, workingTexture(0)
, _ccBands(ccBands)
, iPaletteSize(0)
, rasterLayerModel(rasterLayerModel)
, fColorComposite(true)
{
	textures.resize(1);
}

TextureHeap::TextureHeap(RasterLayerModel * rasterLayerModel, const IRasterCoverage & raster, const unsigned int iPaletteSize, bool asAnimation)
: textureThread(0)
, fAbortTexGen(false)
, fStopThread(false)
, workingTexture(0)
, raster(raster)
, iPaletteSize(iPaletteSize)
, rasterLayerModel(rasterLayerModel)
, fColorComposite(false)
{
	textures.resize(asAnimation ? raster->size().zsize() : 1);
}

TextureHeap::~TextureHeap()
{
	if (textureThread)
	{
		fStopThread = true;
        pauseThread.wakeAll();
        textureThread->wait(); // wait here til thread exits
	}

	ClearQueuedTextures();

	for(int j=0; j < textures.size(); ++j)
		for (int i = 0; i < textures[j].size(); ++i)
			if (textures[j][i] != 0)
			delete textures[j][i];
}

void TextureHeap::ClearQueuedTextures()
{
	fAbortTexGen = true;
	csChangeTexCreatorList.lock(); // wait for TexGen thread to stop
	fAbortTexGen = false;
	for (int i = 0; i < textureRequest.size(); ++i) {
		Texture * tex = textureRequest[i];
			if (tex && !tex->fValid())
				delete tex;
	}
	textureRequest.clear();
	csChangeTexCreatorList.unlock();
}

void TextureHeap::ReGenerateAllTextures()
{
	for (int j = 0; j < textures.size(); ++j)
		for (int i = 0; i < textures[j].size(); ++i)
			textures[j][i]->SetDirty();
}

Texture * TextureHeap::GetTexture(int bandIndex, bool & optimal, const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, unsigned int zoomFactor, bool fInThread)
{
	Texture * tex = 0;
	if (fInThread) { // call Invalidate when done, to redraw the mapwindow
		for (int i = 0; i < textures[bandIndex].size(); ++i) {
			if (textures[bandIndex][i]->equals(bandIndex, offsetX, offsetY, offsetX + sizeX, offsetY + sizeY, zoomFactor)) {
				if (textures[bandIndex][i]->fDirty())
					ReGenerateTexture(textures[bandIndex][i], fInThread);
				else
					optimal = true; 
				return textures[bandIndex][i];
			} else if (textures[bandIndex][i]->contains(bandIndex,offsetX, offsetY, offsetX + sizeX, offsetY + sizeY)) {
				if (tex != 0) {
					if (tex->getZoomFactor() > textures[bandIndex][i]->getZoomFactor())
						tex = textures[bandIndex][i];
				} else
					tex = textures[bandIndex][i];
			}
		}
		// if it is queued already, don't add it again, just be patient as it will come
		csChangeTexCreatorList.lock();
		bool fQueued = workingTexture && workingTexture->equals(bandIndex, offsetX, offsetY, offsetX + sizeX, offsetY + sizeY, zoomFactor);
		if (!fQueued) {
			for (std::vector<Texture*>::iterator it = textureRequest.begin(); it != textureRequest.end() && !fQueued; ++it)
				fQueued = (*it)->equals(bandIndex, offsetX, offsetY, offsetX + sizeX, offsetY + sizeY, zoomFactor);
		}
		csChangeTexCreatorList.unlock();
		if (!fQueued)
			GenerateTexture(bandIndex, offsetX, offsetY, sizeX, sizeY, zoomFactor, fInThread);
	} else { // caller is waiting for the Texture*
		for (int i = 0; i < textures.size(); ++i) {
			if (textures[bandIndex][i]->equals(bandIndex, offsetX, offsetY, offsetX + sizeX, offsetY + sizeY, zoomFactor))
				tex = textures[bandIndex][i];
		}
		if (0 == tex)
			tex = GenerateTexture(bandIndex,offsetX, offsetY, sizeX, sizeY, zoomFactor, fInThread);
		else if (tex->fDirty())
			ReGenerateTexture(tex, fInThread);
	}

	return tex;
}

int TextureHeap::hasOptimalTexture(int bandIndex, const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, unsigned int zoomFactor) const {
	for (int i = 0; i < textures[bandIndex].size(); ++i) {
		if (textures[bandIndex][i]->equals(bandIndex, offsetX, offsetY, offsetX + sizeX, offsetY + sizeY, zoomFactor)) {
			if (textures[bandIndex][i]->fDirty()) {
				return i;
			}
			return TEXAVAILABLE;
		}
	}
	return TEXNONE;
}

bool TextureHeap::findTexture(int bandIndex, const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, unsigned int zoomFactor) {
	auto texIndex = hasOptimalTexture(bandIndex, offsetX, offsetY, sizeX, sizeY, zoomFactor);
	if (texIndex >=0 ) {
		ReGenerateTexture(textures[bandIndex][texIndex], true);
		return false;
	}
	else if (texIndex == TEXAVAILABLE)
		return true;


	// if it is queued already, don't add it again, just be patient as it will come
	csChangeTexCreatorList.lock();
	bool fQueued = workingTexture && workingTexture->equals(bandIndex, offsetX, offsetY, offsetX + sizeX, offsetY + sizeY, zoomFactor);
	if (!fQueued) {
		for (std::vector<Texture*>::iterator it = textureRequest.begin(); it != textureRequest.end() && !fQueued; ++it)
			fQueued = (*it)->equals(bandIndex, offsetX, offsetY, offsetX + sizeX, offsetY + sizeY, zoomFactor);
	}
	csChangeTexCreatorList.unlock();
	if (!fQueued)
		GenerateTexture(bandIndex, offsetX, offsetY, sizeX, sizeY, zoomFactor, true);
	return false;
}

Texture * TextureHeap::GenerateTexture(int bandIndex, const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, unsigned int zoomFactor, bool fInThread)
{
	csChangeTexCreatorList.lock();
	if (fColorComposite)
		textureRequest.push_back(new CCTexture(rasterLayerModel, _ccBands, offsetX, offsetY, sizeX, sizeY, zoomFactor));
	else if (raster.isValid()) {
		textureRequest.push_back(new Texture(rasterLayerModel, raster, bandIndex, offsetX, offsetY, sizeX, sizeY, zoomFactor, iPaletteSize));
	}
	csChangeTexCreatorList.unlock();
	if (fInThread) {
        if (!textureThread) {
            textureThread = new TextureGenerator(this);
            textureThread->connect(textureThread, &TextureGenerator::finished, textureThread, &QObject::deleteLater);
            textureThread->connect(textureThread, &TextureGenerator::requestRedraw, rasterLayerModel, &RasterLayerModel::requestRedraw);
            textureThread->start();
        }
        else
            pauseThread.wakeAll();
	} else {
		Texture * tex = GenerateNextTexture(fInThread);
		if (tex != 0)
			return tex;
	}
	return 0;
}

void TextureHeap::ReGenerateTexture(Texture * texture, bool fInThread)
{
	csChangeTexCreatorList.lock();
	textureRequest.push_back(texture);
	csChangeTexCreatorList.unlock();
	if (fInThread) {
        if (!textureThread) {
            textureThread = new TextureGenerator(this);
            textureThread->connect(textureThread, &TextureGenerator::finished, textureThread, &QObject::deleteLater);
            textureThread->connect(textureThread, &TextureGenerator::requestRedraw, rasterLayerModel, &RasterLayerModel::requestRedraw);
            textureThread->start();
        }
        else
            pauseThread.wakeAll();
	} else
		GenerateNextTexture(fInThread);
}

Texture * TextureHeap::GenerateNextTexture(bool fInThread)
{
	Texture * tex = 0;
	csChangeTexCreatorList.lock();
	if (!fAbortTexGen && !fStopThread && textureRequest.size() > 0) {
		tex = textureRequest.back();
		textureRequest.pop_back();
		workingTexture = tex;
	}
	csChangeTexCreatorList.unlock();

	if (tex != 0) {
		try {
			bool fReGenerate = tex->fValid();
			clock_t start = clock();
			if (fReGenerate)
				tex->ReCreateTexture(fInThread, &fAbortTexGen);
			else
				tex->CreateTexture(fInThread, &fAbortTexGen);
			clock_t end = clock();
			double duration = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
			//TRACE("Texture generated in %2.2f milliseconds;\n", duration);
			if (!fReGenerate) {
				if (tex->fValid())
					textures[tex->bandIndex()].push_back(tex);
				else {
					delete tex;
					tex = 0;
				}
			}
			csChangeTexCreatorList.lock();
			workingTexture = 0;
			csChangeTexCreatorList.unlock();
		} catch (ErrorObject& err) {
			csChangeTexCreatorList.lock();
			workingTexture = 0;
			delete tex;
			tex = 0;
			csChangeTexCreatorList.unlock();
			throw err;
		}
	}
	return tex;
}

TextureGenerator::TextureGenerator(TextureHeap * heap)
: pObject(heap)
{

}

void TextureGenerator::run()
{
	bool fErrorShown = false;

	while (!pObject->fStopThread)
	{
		try {
			clock_t start = clock();
			Texture * tex = pObject->GenerateNextTexture(true);
			while (tex != 0) {
				clock_t end = clock();
				long duration = (end - start) * 1000 / CLOCKS_PER_SEC; // we want this in millisec
				if (duration >= 1000) { // approximately 1 sec between intermediate screen updates
                    emit requestRedraw();
					start = end;
				}
				tex = pObject->GenerateNextTexture(true);
			}
            if (!pObject->fAbortTexGen && !pObject->fStopThread)
                emit requestRedraw();
            if (!pObject->fStopThread) {
                pObject->csThread.lock();
                pObject->pauseThread.wait(&pObject->csThread); // wait here, and dont consume CPU time either
                pObject->csThread.unlock();
            }
		} catch (ErrorObject& err) {
			if (!fErrorShown) {
				fErrorShown = true;
                QString msg = err.message(); // log it?
			}
			pObject->ClearQueuedTextures();
			pObject->fAbortTexGen = true;
		}
	}

	pObject->fStopThread = false;
}

bool TextureHeap::renderingComplete() {
    return (workingTexture == 0) && (textureRequest.size() == 0);
}
