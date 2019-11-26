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
		_featureSpaces.prepare();
	}
}

QColor SupervisedClassification::raw2Color(Raw r) const {
	auto iter = _rprElements.find(r);
	if (iter != _rprElements.end()) {
		return (*iter).second->color();
	}
	return QColor();
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
	if (key == CALCITEMSTATS) {
		calcStats(var.toDouble());
		//clearSelection();
	}
}

QVariantList SupervisedClassification::bandstats(qint32 r) const {
	QVariantList data;
	if (_bbSelection.isValid()) {
		auto stat2 = _stats.find(r);
		if (stat2 != _stats.end()) {
			const ClassificationEntry& v = (*stat2).second;
			const std::vector<std::vector<double>>& s = v.second;
			QVariantMap mp;
			for (int z = 0; z < _multiSpectralRaster->size().zsize(); ++z) {
				mp["name"] = "band " + QString::number(z);
				mp["mean"] = QString::number(s[z][stMEAN]);
				mp["predominant"] = QString::number(s[z][stPREDOM]);
				mp["stdev"] = QString::number(s[z][stSTDDEV]);
				mp["count"] = QString::number(s[z][stCOUNT]);
				mp["raw"] = r;
				data.push_back(mp);
			}
		}
	}
	return data;

}

QVariantMap SupervisedClassification::stretchLimits() const {
	QVariantMap result;
	if (_multiSpectralRaster.isValid()) {
		if (!_multiSpectralRaster->histogramCalculated()){
			_multiSpectralRaster->statistics(PIXELVALUE, ContainerStatistics<PIXVALUETYPE>::pQUICKHISTOGRAM).histogram();
		}
		auto pair = _multiSpectralRaster->statistics(PIXELVALUE).calcStretchRange(0.02);
		result["min"] = pair.first;
		result["max"] = pair.second;
	}
	return result;
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
		if (_items.isValid()) {
			for (auto item : _items) {
				QVariantMap vitem;
				vitem["name"] = item->name();
				vitem["raw"] = item->raw();
				result.push_back(vitem);
			}
			data = result;
		}
	}
	if (key == TABLEID) {
		return _featureSpaces->id();
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
		_bbSelection.min_corner().z = 0;
		_bbSelection.max_corner().z = 0;
		clearMarked();
		calcStats(-1);

	}
	return count;
}

void SupervisedClassification::calcBasic(const BoundingBox& box, int z, double rw, std::vector<double>& stats, std::map<double, quint32>& predom) const{
	quint32 count = 0;
	double sum = 0;

	auto CalcB = [&](PIXVALUETYPE v, double& sum, quint32& count)->void {
		++count;
		if (v != rUNDEF) {
			sum += v;
			predom[v]++;
		}
	};

	BoundingBox bbBand = box;
	BoundingBox bbBase = box;
	bbBand.min_corner().z = z;
	bbBand.max_corner().z = z;
	PixelIterator iterSelect(_multiSpectralRasterSelection, bbBase);
	PixelIterator iter(_multiSpectralRaster, bbBand);
	PixelIterator iterClassMap(_classRaster, bbBase);
	auto send = iterSelect.end();
	while (iterSelect != send) {
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
		//if (rw != -1)
		//	selectV = rUNDEF;
	};

	BoundingBox bbBase = box;
	bbBase.min_corner().z = 0;
	bbBase.max_corner().z = 0;
	PixelIterator iterSelect2(_multiSpectralRasterSelection, bbBase);
	PixelIterator iter2(_multiSpectralRaster, box);
	PixelIterator iterClassMap(_classRaster, bbBase);
	auto send = iterSelect2.end();
	while (iterSelect2 != send) {
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
	BoundingBox newBox;
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
	if (!isCopy && newBox.isValid())
		result.push_back(newBox);
	return result;

}

void SupervisedClassification::calcStats(Raw rw) {
	if (_bbSelection.isValid()) {
		std::map<double, quint32> predom;
		std::vector<BoundingBox> mergedBoxes;
		_stats[rw].second.resize(_multiSpectralRaster->size().zsize());
		if (rw != -1) {
			ClassificationEntry& entry = _stats[rw];
			std::vector<BoundingBox>& boxes = entry.first;
			auto iter = std::find(boxes.begin(), boxes.end(), _bbSelection);
			if (iter == boxes.end())
				boxes.push_back(_bbSelection);
			mergedBoxes = mergeBoxes(boxes);
		}

		for (int z = 0; z < _multiSpectralRaster->size().zsize(); ++z) {
			std::vector<double> stats(5, 0);
			if (rw == -1) {
				double var = 0;
				BoundingBox bb = _bbSelection;
				calcBasic(bb, z, rw, stats, predom);
				if (stats[stCOUNT] == 0)
					return;

				calcVariance(bb, rw, stats, var);
				double stdev = stats[stCOUNT] > 1 ? std::sqrt(var / (stats[stCOUNT] - 1)) : 0;
				stats[stSTDDEV] = stdev;

				findPredom(predom, stats);
			}
			else {
				double var = 0;
				for (auto& box : mergedBoxes) {
					calcBasic(box, z, rw, stats, predom);


					calcVariance(_bbSelection, rw, stats, var);
				}
				if (stats[stCOUNT] == 0)
					return;

				double stdev = stats[stCOUNT] > 1 ? std::sqrt(var / (stats[stCOUNT] - 1)) : 0;
				stats[stSTDDEV] = stdev;
				findPredom(predom, stats);
			}

			_stats[rw].second[z] = stats;
		}
	}
}

void SupervisedClassification::calcFeatureSpace(int bandX, int bandY)  {

	quint32 maxRaw = 0, minRaw = 100000000;
	for (const auto& item : _items) {
		maxRaw = std::max(item->raw(), maxRaw);
		minRaw = std::min(item->raw(), minRaw);
	}
	FeatureSpaceAccumulator b1b2perraw(maxRaw - minRaw + 2);
	for (const auto& item : _items) {
		Raw r = item->raw();
		calcFSperValue(r, bandX, bandY, b1b2perraw, r - minRaw); // class training pixels
	}
	calcFSperValue(-1, bandX, bandY, b1b2perraw, b1b2perraw.size() - 1); // selection

	_featureSpaces->clearTable(true);
	for (int r = 0; r < b1b2perraw.size(); ++r) {
		auto& pairs = b1b2perraw[r];
		if (pairs.size() > 0) {
			int rec = 0;
			auto columns = columnIndexes(r + minRaw, bandX, bandY);
			for (auto& b1b2Combo : pairs) {
				_featureSpaces->setCell(columns.first, rec, b1b2Combo.first);
				_featureSpaces->setCell(columns.second, rec, b1b2Combo.second);
				++rec;
			}
		}
	}
}

void Ilwis::Ui::SupervisedClassification::calcFSperValue(Raw r, int bandX, int bandY, FeatureSpaceAccumulator &b1b2perraw, const quint32 b1b2Index)
{
	auto iter = _stats.find(r);
	if (iter != _stats.end()) {
		bool useSelection = b1b2Index == b1b2perraw.size() - 1;
		ClassificationEntry& entry = (*iter).second;
		std::vector<BoundingBox>& boxes = entry.first;
		std::vector<BoundingBox> mergedBoxes;
		if (useSelection) {
			if (_bbSelection.isValid()) {
				mergedBoxes.push_back(_bbSelection);
			}
		}
		else
			mergedBoxes = mergeBoxes(boxes);

		for (auto& box : mergedBoxes) {
			BoundingBox bbBaseX = box;
			BoundingBox bbBaseY = box;
			bbBaseX.min_corner().z = bandX;
			bbBaseX.max_corner().z = bandX;
			bbBaseY.min_corner().z = bandY;
			bbBaseY.max_corner().z = bandY;
			PixelIterator iterX(_multiSpectralRaster, bbBaseX);
			PixelIterator iterY(_multiSpectralRaster, bbBaseY);
			PixelIterator iterClassMap(useSelection ? _multiSpectralRasterSelection : _classRaster, box);
			auto xend = iterX.end();
			while (iterX != xend) {
				Raw raw = (*iterClassMap);
				if (raw != rUNDEF) {
					int vx = (*iterX);
					int vy = (*iterY);
					if (vx != rUNDEF && vy != rUNDEF) {
						b1b2perraw[b1b2Index].push_back(std::pair<int, int>(vx, vy));
					}
				}
				++iterX;
				++iterY;
				++iterClassMap;
			}
		}
	}
}

QVariantList SupervisedClassification::tableColumns(int bandX, int bandY) const {
	QVariantList result;
	QString baseNameX = QString("band_%1_").arg(bandX);
	QString baseNameY = QString("band_%1_").arg(bandY);
	QString yName;
	QVariantMap data;
	for (int c = 0; c < _featureSpaces->columnCount(); ++c) {
		if (_featureSpaces->columndefinitionRef(c).name().indexOf(baseNameX) == 0) {
			data["xband"] = c;
			data["xbandname"] = _featureSpaces->columndefinitionRef(c).name();
			data["minx"] = _featureSpaces->columndefinitionRef(c).datadef().range<NumericRange>()->min();
			data["maxx"] = _featureSpaces->columndefinitionRef(c).datadef().range<NumericRange>()->max();
			auto parts = data["xbandname"].toString().split("_");
			QString raw = parts[2];
			yName = baseNameY + raw;
			for (int yc = 0; yc < _featureSpaces->columnCount(); ++yc) {
				if (_featureSpaces->columndefinitionRef(yc).name().indexOf(yName) == 0) {
					data["yband"] = yc;
					data["ybandname"] = yName;
					data["miny"] = _featureSpaces->columndefinitionRef(yc).datadef().range<NumericRange>()->min();
					data["maxy"] = _featureSpaces->columndefinitionRef(yc).datadef().range<NumericRange>()->max();
					result.push_back(data);
				}
			}
			
		}
	}
	return result;
}

std::pair<int, int> SupervisedClassification::columnIndexes(Raw raw, int bandX, int bandY) {
	bool comboExists = false;

	comboExists = true;
	QString bandNameX = QString("band_%1_%2").arg(bandX).arg(raw);
	QString bandNameY = QString("band_%1_%2").arg(bandY).arg(raw);
	int columnIndexX = _featureSpaces->columnIndex(bandNameX);
	int columnIndexY = _featureSpaces->columnIndex(bandNameY);

	if (columnIndexX == iUNDEF) {
		columnIndexX = _featureSpaces->columnCount();
		_featureSpaces->addColumn(bandNameX, IDomain("integer"));
	}
	if (columnIndexY == iUNDEF) {
		columnIndexY = _featureSpaces->columnCount();
		_featureSpaces->addColumn(bandNameY, IDomain("integer"));
	}
	return std::pair<int, int>(columnIndexX, columnIndexY);
}





