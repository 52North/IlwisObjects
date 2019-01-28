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
#include "modeller/modellerfactory.h"       
#include "commandhandler.h" 
#include "operationmetadata.h"   
#include "raster.h"
#include "pixeliterator.h"
#include "operationhelpergrid.h"
#include "box.h" 
#include "supervisedclassification.h"      

using namespace Ilwis;
using namespace Ui;      


SupervisedClassification::SupervisedClassification() : Ilwis::AnalysisPattern()
{
    Init(); 
}

void SupervisedClassification::Init()     
{
}

bool SupervisedClassification::execute(const QVariantMap &inputParameters, QVariantMap &outputParameters)
{
  return true;
}

IWorkflow SupervisedClassification::workflow(const IOOptions &opt)
{
    return IWorkflow();
}

void SupervisedClassification::store(QDataStream &stream)
{
    stream << type();
    AnalysisPattern::store(stream);
	QString raster = _multiSpectralRaster.isValid() ? _multiSpectralRaster->resource().url().toString() : "";
	stream << raster;
}

void SupervisedClassification::loadMetadata(QDataStream &stream)
{
    AnalysisPattern::loadMetadata(stream);
	QString sraster;
	stream >> sraster;
	if (sraster != "") {
		if (!_multiSpectralRaster.prepare(sraster)) {
			kernel()->issues()->log(TR("Couldnt load model. Raster is not available: ") + sraster);
			return;
		}
		OperationHelperRaster::initialize(_multiSpectralRaster, _multiSpectralRasterSelection, itDOMAIN | itGEOREF | itCOORDSYSTEM | itENVELOPE);
		std::vector<double> indexes = { 0 };
		_multiSpectralRasterSelection->setDataDefintions(IDomain("count"), indexes);
		_multiSpectralRasterSelection->name("PixelSelection");
	}
}

QString SupervisedClassification::type() const
{
    return "supervisedclassification";
}

SupervisedClassification::SupervisedClassification(const QString &name, const QString &description) : AnalysisPattern(name, description)
{
    Init(); 
}

SupervisedClassification::AnalysisPattern *SupervisedClassification::create(const QString &name, const QString &description, const IOOptions &options)
{
    SupervisedClassification *sc =  new SupervisedClassification(name, description);
	if (options.contains("multispectralraster")) {
		QString sraster = options["multispectralraster"].toString();
		IRasterCoverage raster;
		if (raster.prepare(sraster)) {
			QVariant data;
			data.setValue<IRasterCoverage>(raster);
			sc->addData(SCRASTERKEY, data);
		}
	}
    return sc;
}

void SupervisedClassification::addData(const QString& key, const QVariant& var) {

	if (key == SCRASTERKEY) {
		IRasterCoverage raster = var.value<IRasterCoverage>();
		if (raster.isValid()) {
			_multiSpectralRaster = raster;
			OperationHelperRaster::initialize(raster, _multiSpectralRasterSelection, itDOMAIN | itGEOREF | itCOORDSYSTEM | itENVELOPE);
			std::vector<double> indexes = { 0 };
			_multiSpectralRasterSelection->setDataDefintions(IDomain("count"), indexes);
			_multiSpectralRasterSelection->name("PixelSelection");
		}
	}
	if (key == SELECTEDPIXEL) {
		QVariantMap point = var.toMap();
		Pixel p{ point["x"].toInt(), point["y"].toInt() };
		if (p != _selectionPoint) {
			_selectionPoint = p;
			setSelection(_selectionPoint);
		}
	}
	if (key == SPECTRALDISTANCE) {
		auto d = var.toDouble();
		if (d != _spectralDistance) {
			_spectralDistance = d;
			setSelection(_selectionPoint);
		}
	}
}

QVariant SupervisedClassification::data(const QString& key) const {
	QVariant data;
	if ( key == SCRASTERKEY)
		data.setValue<IRasterCoverage>(_multiSpectralRaster);
	if ( key == SELECTIONRASTERKEY)
		data.setValue<IRasterCoverage> (_multiSpectralRasterSelection);
	return data;
}

void SupervisedClassification::generateNeighbours(const Pixel& pix, PixelIterator& iterSelect, std::vector<Pixel>& linPixelPositions) {

	std::vector<Pixel> basePositions;
	basePositions.reserve(9);
	std::vector<int> offsets = { 1,0,-1 };
	for (int x = 0; x < 3; ++x) {
		auto newX = pix.x + offsets[x];
		if (newX >= 0 || newX < iterSelect.raster()->size().xsize()) {
			for (int y = 0; x < y; ++y) {
				auto newY = pix.y + offsets[y];
				if (newY >= 0 || newY < iterSelect.raster()->size().ysize())
					basePositions.push_back(Pixel(newX, newY));
			}
		}
	}

	for (const Pixel& basePix : basePositions) {
		auto& neighbourLocation = *(iterSelect[basePix]);
		if (neighbourLocation != MARKED && neighbourLocation != SELECT) {
			linPixelPositions.push_back(basePix);
			neighbourLocation = MARKED;
		}
	}
}

void SupervisedClassification::setSelection(const Pixel& pix) {
	if (_spectralDistance == rUNDEF || _spectralDistance <=0 || !_selectionPoint.isValid() ||
		!_multiSpectralRasterSelection.isValid() || !_multiSpectralRaster.isValid())
		return;

	PixelIterator iterSelect(_multiSpectralRasterSelection);
	PixelIterator iterInput(_multiSpectralRaster);
	iterSelect = pix;
	iterInput = pix;
	double d2 = _spectralDistance * _spectralDistance;
    double startValue = *iterInput;
	startValue = startValue * startValue;
	std::vector<Pixel> linPixelPositions;

	generateNeighbours(iterInput.position(), iterSelect, linPixelPositions);

	while (linPixelPositions.size() > 0) {
		Pixel linPos = linPixelPositions.back();
		linPixelPositions.pop_back();
		double vInput = *(iterInput[linPos]);
		double vInput2 = vInput * vInput;
		if (startValue - vInput2 <= d2) {
			*(iterSelect[linPos]) = SELECT;
			generateNeighbours(linPos, iterSelect, linPixelPositions);
		}
	}
}



