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


#include "cctexture.h"
#include "pixeliterator.h"
#include "rasterlayermodel.h"
#include "core/ilwisobjects/domain/itemrange.h"
#include "core/ilwisobjects/domain/colorrange.h"
#include "ilwiscoreui/coveragedisplay/visualattribute.h"

using namespace Ilwis;
using namespace Ui;

CCTexture::CCTexture(RasterLayerModel * rasterLayerModel, const std::vector<IRasterCoverage> * ccBands, const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, unsigned int zoomFactor)
: Texture(rasterLayerModel, ccBands->at(0), offsetX, offsetY, sizeX, sizeY, zoomFactor, 0)
, _ccBands(ccBands)
{
}

CCTexture::~CCTexture()
{
}

void CCTexture::CreateTexture(bool fInThread, volatile bool * fDrawStop)
{
    valid = DrawTexture(offsetX, offsetY, sizeX, sizeY, zoomFactor, texture_data, fDrawStop);
}

void CCTexture::ReCreateTexture(bool fInThread, volatile bool * fDrawStop)
{
    dirty = !DrawTexture(offsetX, offsetY, sizeX, sizeY, zoomFactor, texture_data, fDrawStop);
}

bool CCTexture::DrawTexture(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, QVector<int> & texture_data, volatile bool* fDrawStop)
{
    long imageWidth = _ccBands->at(0)->size().xsize();
    long imageHeight = _ccBands->at(0)->size().ysize();
    long sizeX = texSizeX; // the size of the input (pixeliterator)
    long sizeY = texSizeY;
    if (offsetX + sizeX > imageWidth)
        sizeX = imageWidth - offsetX;
    if (offsetY + sizeY > imageHeight)
        sizeY = imageHeight - offsetY;
    if (sizeX == 0 || sizeY == 0)
        return false;
    const long xSizeOut = (long)ceil((double)sizeX / ((double)zoomFactor)); // the size until which the pixels vector will be filled (this is commonly the same as texSizeX, except the rightmost / bottommost textures, as raster-images seldom have as size of ^2)
    texSizeX /= zoomFactor; // the actual size of the texture (commonly 256 or maxtexturesize, but smaller texture sizes may be allocated for the rightmost or bottommost textures)
    texSizeY /= zoomFactor;

    if (*fDrawStop)
        return false;

    BoundingBox bbR(Pixel(offsetX, offsetY, 0), Pixel(offsetX + sizeX - 1, offsetY + sizeY - 1, 0));
    BoundingBox bbG(Pixel(offsetX, offsetY, 0), Pixel(offsetX + sizeX - 1, offsetY + sizeY - 1, 0));
    BoundingBox bbB(Pixel(offsetX, offsetY, 0), Pixel(offsetX + sizeX - 1, offsetY + sizeY - 1, 0));
    quint32 size = texSizeX * texSizeY * 4; // r,g,b,a
    if (texture_data.size() == 0)
        texture_data.resize(size); // allowed the first time only; after this the vector will always be in-use by a webGL object

    if (*fDrawStop)
        return false;

    VisualAttribute * attr = _rasterLayerModel->layer(0)->activeAttribute();
	if (attr != 0) {
		NumericRange & actualRange = attr->actualRange();
		NumericRange & stretchRange = attr->stretchRange();
		quint32 position = 0; // red
		PixelIterator pixIterR(_ccBands->at(0), bbR); // This iterator runs through bbR. The corners of bbR are "inclusive".
		auto endR = pixIterR.end();
		while (pixIterR != endR && position < size) {
			double valueR = *pixIterR;
			int colorR = isNumericalUndef2(valueR, _ccBands->at(0)) ? 0 : 255.0 * getStretchedValue(valueR, actualRange, stretchRange);
			texture_data[position] = colorR;
			position += 3;
			texture_data[position++] = 255; // alpha
			pixIterR += zoomFactor;
			if (pixIterR.ychanged()) {
				if (*fDrawStop)
					return false;

				position += 4 * (texSizeX - xSizeOut);
				if (zoomFactor > 1) {
					pixIterR += sizeX * (zoomFactor - 1) - ((zoomFactor - (sizeX % zoomFactor)) % zoomFactor);
				}
			}
		}
	}
	attr = _rasterLayerModel->layer(1)->activeAttribute();
	if (attr != 0) {
		NumericRange & actualRange = attr->actualRange();
		NumericRange & stretchRange = attr->stretchRange();
		quint32 position = 1; // green
		PixelIterator pixIterG(_ccBands->at(1), bbG); // This iterator runs through bbG. The corners of bbG are "inclusive".
		auto endG = pixIterG.end();
		while (pixIterG != endG && position < size) {
			double valueG = *pixIterG;
			int colorG = isNumericalUndef2(valueG, _ccBands->at(1)) ? 0 : 255.0 * getStretchedValue(valueG, actualRange, stretchRange);
			texture_data[position] = colorG;
			position += 4;
			pixIterG += zoomFactor;
			if (pixIterG.ychanged()) {
				if (*fDrawStop)
					return false;

				position += 4 * (texSizeX - xSizeOut);
				if (zoomFactor > 1) {
					pixIterG += sizeX * (zoomFactor - 1) - ((zoomFactor - (sizeX % zoomFactor)) % zoomFactor);
				}
			}
		}
	}
	attr = _rasterLayerModel->layer(2)->activeAttribute();
	if (attr != 0) {
		NumericRange & actualRange = attr->actualRange();
		NumericRange & stretchRange = attr->stretchRange();
		quint32 position = 2; // blue
        PixelIterator pixIterB(_ccBands->at(2), bbB); // This iterator runs through bbB. The corners of bbB are "inclusive".
        auto endB = pixIterB.end();
        while (pixIterB != endB && position < size) {
            double valueB = *pixIterB;
            int colorB = isNumericalUndef2(valueB, _ccBands->at(2)) ? 0 : 255.0 * getStretchedValue(valueB, actualRange, stretchRange);
            texture_data[position] = colorB;
            position += 4;
            pixIterB += zoomFactor;
            if (pixIterB.ychanged()) {
                if (*fDrawStop)
                    return false;

                position += 4 * (texSizeX - xSizeOut);
                if (zoomFactor > 1) {
                    pixIterB += sizeX * (zoomFactor - 1) - ((zoomFactor - (sizeX % zoomFactor)) % zoomFactor);
                }
            }
        }
    }
	return true;
}
