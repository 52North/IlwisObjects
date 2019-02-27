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
#include <QColor>
#include "ilwisdata.h"   
#include "modeller/modellerfactory.h"       
#include "commandhandler.h" 
#include "operationmetadata.h"   
#include "raster.h"
#include "pixeliterator.h"
#include "operationhelpergrid.h"
#include "box.h" 
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "representation.h"
#include "representationelementmodel.h"
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

	QString dom = _items.isValid() ? _items->resource().url().toString() : "";
	stream << dom;
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
		OperationHelperRaster::initialize(_multiSpectralRaster, _multiSpectralRasterSelection, itGEOREF | itCOORDSYSTEM | itENVELOPE);
		std::vector<double> indexes = { 0 };
		_multiSpectralRasterSelection->setDataDefintions(IDomain("integer"), indexes);
		_multiSpectralRasterSelection->name("PixelSelection");
	}
	QString dom;
	stream >> dom;
	if (dom != "") {
		if (!_items.prepare(dom)) {
			kernel()->issues()->log(TR("Couldnt load model. Domain is not available: ") + dom);
			return;
		}
		OperationHelperRaster::initialize(_multiSpectralRaster, _classRaster, itGEOREF | itCOORDSYSTEM | itENVELOPE);
		std::vector<double> indexes = { 0 };
		_classRaster->setDataDefintions(_items, indexes);
		_classRaster->name("ClassRaster");
		IFlatTable tbl;
		tbl.prepare();
		tbl->addColumn(COVERAGEKEYCOLUMN, _items);
		int rec = 0;
		ItemRangeIterator iter(_classRaster->datadef().domain()->range<>().data());
		while (iter.isValid()) {
			SPDomainItem item = (*iter);
			tbl->setCell(0, rec++, item->raw());
			++iter;
		}
		_classRaster->setAttributes(tbl);
		IRepresentation representation = Representation::defaultRepresentation(_items);
		if (_rprElements.size() == 0 && representation.isValid()) {
			for (auto item : _items) {
					QColor clr = representation->colors()->value2color(item->raw());
					auto *elem = new RepresentationElementModel(representation, item->raw(), item->name(), this);
					elem->color(representation->colors()->value2color(item->raw()));
					_rprElements[item->raw()] = elem;
			}
		}
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
	if (options.contains("classificationdomain")) {
		QString dom = options["classificationdomain"].toString();
		if (dom!= "") {
			QVariant data;
			data = dom;
			sc->addData(SCDOMAINKEY, data);
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
			_multiSpectralRasterSelection->setDataDefintions(IDomain("integer"), indexes);
			_multiSpectralRasterSelection->name("PixelSelection");
		}
	}
	if (key == SELECTEDPIXEL) {
		QVariantMap point = var.toMap();
		Pixel p{ point["x"].toInt(), point["y"].toInt() };
		if (p != _selectionPoint) {
			clearSelection();
			_selectionPoint = p;
			setSelection();
		}
	}
	if (key == SPECTRALDISTANCE) {
		auto d = var.toDouble();
		if (d != _spectralDistance) {
			clearSelection();
			_spectralDistance = d;
			setSelection();
		}
	}
	if (key == SCDOMAINKEY) {
		auto d = var.toString();
		_items.prepare(d, itDOMAIN, { "mustexist", true });
		OperationHelperRaster::initialize(_multiSpectralRaster, _classRaster, itGEOREF | itCOORDSYSTEM | itENVELOPE);
		std::vector<double> indexes = { 0 };
		_classRaster->setDataDefintions(_items, indexes);
		_classRaster->name("ClassRaster");
		IRepresentation representation = Representation::defaultRepresentation(_items);
		if (_rprElements.size() == 0) {
			for (auto item : _items) {
					QColor clr = representation->colors()->value2color(item->raw());
					auto *elem = new RepresentationElementModel(representation, item->raw(), item->name(), this);
					elem->color(representation->colors()->value2color(item->raw()));
					_rprElements[item->raw()] = elem;
			}
		}
	}
	if (key == ITEMSTATS) {
		calcStats(var.toDouble());
	}
}

QVariant SupervisedClassification::data(const QString& key) const {
	QVariant data;
	if ( key == SCRASTERKEY)
		data.setValue<IRasterCoverage>(_multiSpectralRaster);
	if ( key == SELECTIONRASTERKEY)
		data.setValue<IRasterCoverage> (_multiSpectralRasterSelection);
	if (key == CLASSRASTER)
		data.setValue<IRasterCoverage>(_classRaster);
	if (key == SCDOMAINKEY) {
		if (_items.isValid()) {
			data =  _items->resource().url().toString();
		}
	}
	if (key == SCDOMAINID) {
		if (_items.isValid())
			data = QString::number(_items->id());
	}
	if (key == CLASSIFIERITEMS) {
		QVariantList result;
		QVariantMap selectedPixels;
		selectedPixels["name"] = TR("Selected Pixels");
		auto stat = _stats.find(-1);
		bool found = stat != _stats.end();
		selectedPixels["mean"] = found ? QString::number((*stat).second.second[stMEAN]) : sUNDEF;
		selectedPixels["predominant"] = found ? QString::number((*stat).second.second[stPREDOM]) : sUNDEF;
		selectedPixels["stdev"] = found ? QString::number((*stat).second.second[stSTDDEV]) : sUNDEF;
		selectedPixels["count"] = found ? QString::number((*stat).second.second[stCOUNT]) : "0";
		selectedPixels["raw"] = -1;
		selectedPixels["color"] = QColor("white");
		result.push_back(selectedPixels);

		if (_items.isValid()) {
			for (auto item : _items) {
				QVariantMap mp;
				double r = item->raw();
				mp["name"] = item->name();
				auto stat = _stats.find(r);
				bool found = stat != _stats.end();
				mp["mean"] = found ? QString::number((*stat).second.second[stMEAN]) : sUNDEF;
				mp["predominant"] = found ? QString::number((*stat).second.second[stPREDOM]) : sUNDEF;
				mp["stdev"] = found ? QString::number((*stat).second.second[stSTDDEV]) : sUNDEF;
				mp["count"] = found ? QString::number((*stat).second.second[stCOUNT]) : "0";
				mp["raw"] = r;
				auto iter = _rprElements.find(r);
				if (iter != _rprElements.end()) {
					mp["color"] = (*iter).second->color();
				}

				result.push_back(mp);
			}
			data = result;
		}
	}
	return data;
}

void SupervisedClassification::clearSelection() {
	if (_bbSelection.isValid()) {
		PixelIterator iterSelect(_multiSpectralRasterSelection, _bbSelection);
		while (iterSelect != iterSelect.end()) {
			(*iterSelect) = rUNDEF;
			++iterSelect;
		}
		_bbSelection = BoundingBox();
	}
}

void SupervisedClassification::clearMarked() {
	if (_bbSelection.isValid()) {
		PixelIterator iterSelect(_multiSpectralRasterSelection, _bbSelection);
		while (iterSelect != iterSelect.end()) {
			double & v = (*iterSelect);
			if (v == MARKED)
				v = rUNDEF;
			++iterSelect;
		}
	}
}

void SupervisedClassification::generateNeighbours(const Pixel& pix, PixelIterator& iterSelect, std::vector<Pixel>& linPixelPositions) {

	std::vector<Pixel> basePositions;
	makeBasePositions(pix, basePositions);
	

	for (const Pixel& basePix : basePositions) {
		auto& neighbourLocation = *(iterSelect[basePix]);
		if (neighbourLocation != MARKED && neighbourLocation != SELECT) {
			linPixelPositions.push_back(basePix);
			neighbourLocation = MARKED;
		}
	}
}

void SupervisedClassification::makeBasePositions(const Pixel& pix, std::vector<Pixel>& basePositions) {
	basePositions.reserve(9);
	std::vector<int> offsets = { 1,0,-1 };
	for (int x = 0; x < 3; ++x) {
		auto newX = pix.x + offsets[x];
		if (newX >= 0 || newX < _multiSpectralRasterSelection->size().xsize()) {
			for (int y = 0; y < 3; ++y) {
				auto newY = pix.y + offsets[y];
				if (newY >= 0 || newY < _multiSpectralRasterSelection->size().ysize())
					basePositions.push_back(Pixel(newX, newY, 0));
			}
		}
	}
}
int SupervisedClassification::setSelection() {
	if (_spectralDistance == rUNDEF || _spectralDistance <=0 || !_selectionPoint.isValid() ||
		!_multiSpectralRasterSelection.isValid() || !_multiSpectralRaster.isValid())
		return 0;

	PixelIterator iterSelect(_multiSpectralRasterSelection);
	PixelIterator iterInput(_multiSpectralRaster);
	iterSelect = _selectionPoint;
	*iterSelect = SELECT;
	iterInput = _selectionPoint;
	double d2 = _spectralDistance * _spectralDistance;
    double startValue = *iterInput;
	startValue = startValue * startValue;
	_bbSelection += _selectionPoint;
	std::vector<Pixel> linPixelPositions;

	generateNeighbours(iterInput.position(), iterSelect, linPixelPositions);
	int count = 1; // == 1 because the selected pixel is always by definition part of this set
	while (linPixelPositions.size() > 0) {
		Pixel linPos = linPixelPositions.back();
		linPixelPositions.pop_back();
		double vInput = *(iterInput[linPos]);
		double vInput2 = vInput * vInput;
		if (isNumericalUndef(vInput))
			continue;
		if (std::abs(startValue - vInput2) <= d2) {
			*(iterSelect[linPos]) = SELECT;
			_bbSelection += linPos;
			++count;
			generateNeighbours(linPos, iterSelect, linPixelPositions);
		}
	}
	if (count > 0) {
		auto * range = _multiSpectralRasterSelection->datadef().range()->as<NumericRange>();
		range->min(-1);
		range->max(1);
		Size<> sz = _multiSpectralRasterSelection->size();
		_bbSelection.min_corner().x -= (_bbSelection.min_corner().x > 0 ? 2 : 0);
		_bbSelection.min_corner().y -= (_bbSelection.min_corner().y > 0 ? 2 : 0);
		_bbSelection.max_corner().x += (_bbSelection.max_corner().x < sz.xsize()-1 ? 2 : 0);
		_bbSelection.max_corner().y += (_bbSelection.max_corner().y < sz.ysize()-1 ? 2 : 0);
		clearMarked();
		calcStats(-1);

	}
	return count;
}

void SupervisedClassification::calcBasic(const BoundingBox& box, double rw, std::vector<double>& stats, std::map<double, quint32>& predom) const{
	quint32 count = 0;
	double sum = 0;
	auto CalcB = [&](PIXVALUETYPE v, double& sum, quint32& count)->void {
		++count;
		if (v != rUNDEF) {
			sum += v;
			predom[v]++;
		}
	};

	PixelIterator iterSelect(_multiSpectralRasterSelection, box);
	PixelIterator iter(_multiSpectralRaster, box);
	PixelIterator iterClassMap(_classRaster, box);
	while (iterSelect != iterSelect.end()) {
		double s = (*iterSelect);

		if (s == SELECT) {
			CalcB((*iter), sum, count);
		}
		else if (rw != -1) { // if not the stats of only the selection are asked we also should take into account the blocks that belong to this raw value
			double classV = (*iterClassMap);
			if ( rw == classV)
				CalcB((*iter), sum, count);
		}
		++iterClassMap;
		++iterSelect;
		++iter;
	}
	if (count == 0)
		return;
	stats[stSUM] += sum;
	stats[stCOUNT] += count;
	stats[stMEAN] = stats[stSUM] / stats[stCOUNT];
}

void SupervisedClassification::calcVariance(const BoundingBox& box, Raw rw, const std::vector<double>& stats, double& var) const {

	auto CalcVar = [&](PIXVALUETYPE baseV, Raw rw, double& var, double& classV, double& selectV)->void {
		double v = baseV;
		double delta = v - stats[stMEAN];
		var += delta * delta;
		if (rw != -1) {
			classV = rw;
		}
		if (rw != -1)
			selectV = rUNDEF;
	};
	PixelIterator iterSelect2(_multiSpectralRasterSelection, box);
	PixelIterator iter2(_multiSpectralRaster, box);
	PixelIterator iterClassMap(_classRaster, box);
	while (iterSelect2 != iterSelect2.end()) {
		double& s = (*iterSelect2);
		double& classV = (*iterClassMap);
		if (s == SELECT) {
			CalcVar((*iter2), rw, var,classV, s);
		}else if (classV == rw) {
			CalcVar((*iter2), rw, var, classV, s);
		}
		++iterSelect2;
		++iter2;
		++iterClassMap;
	}
}

void SupervisedClassification::findPredom(const std::map<double, quint32>& predom, std::vector<double>& stats) const {
	quint32 maxCount = 0;
	double pred = 0;
	qint64 pcount = 0;
	for (auto va : predom) {
		if (va.second > maxCount) {
			pred = va.first;
			maxCount = va.second;
		}
		pcount += va.first;
	}

	stats[stPREDOM] = pred;
}

std::vector<BoundingBox> SupervisedClassification::mergeBoxes(const std::vector<BoundingBox>& boxes) const {
	std::vector<BoundingBox> result;
	std::vector<bool> flagged(boxes.size(), false);
	BoundingBox newBox = _bbSelection;
	bool isCopy = false;
	for (int index = 0; index < flagged.size(); ++index) {
		auto box = boxes[index];
		if (box.intersects(newBox)) {
			if (box != newBox) {
				newBox += box;
				flagged[index] = true;
			}
			else {
				isCopy = true;
				break;
			}
		}
	}
	for (int index = 0; index < flagged.size(); ++index) {
		if (!flagged[index]) {
			result.push_back(boxes[index]);
		}
	}
	if (!isCopy)
		result.push_back(newBox);
	return result;

}

void SupervisedClassification::calcStats(Raw rw) {
	if (_bbSelection.isValid()) {
		std::vector<double> stats(5,0);
		std::map<double, quint32> predom;
		if (rw == -1) {
			double var = 0;
			calcBasic(_bbSelection, rw, stats, predom);
			if (stats[stCOUNT] == 0)
				return;

			calcVariance(_bbSelection, rw, stats, var);
			double stdev = std::sqrt(var / (stats[stCOUNT] - 1));
			stats[stSTDDEV] = stdev;

			findPredom(predom, stats);
		}
		else {
			ClassificationEntry& entry = _stats[rw];
			std::vector<BoundingBox>& boxes = entry.first;
			boxes = mergeBoxes(boxes);
			double var = 0;
			for (auto& box : boxes) {
				calcBasic(box, rw, stats, predom);


				calcVariance(_bbSelection, rw, stats, var);
			}
			if (stats[stCOUNT] == 0)
				return;

			double stdev = std::sqrt(var / (stats[stCOUNT] - 1));
			stats[stSTDDEV] = stdev;
			findPredom(predom, stats);
		}
	
		_stats[rw].second = stats;
	}
}





