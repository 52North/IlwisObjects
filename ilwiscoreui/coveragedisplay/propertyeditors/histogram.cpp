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
#include "representation.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "ilwisinterfaces.h"

#include "geos/geom/PrecisionModel.h"
#include "geos/algorithm/locate/SimplePointInAreaLocator.h"
#include "geos/geom/Point.h"
#include "geos/geom/Polygon.h"
#ifdef Q_OS_WIN
#include "geos/geom/PrecisionModel.h"
#include "geos/geom/Envelope.h"
#include "geos/geom/Coordinate.h"
#endif
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/GeometryFactory.h"

#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "raster.h"
#include "coveragelayermodel.h"
#include "table.h"
#include "ilwiscontext.h"
#include "mathhelper.h"
#include "chartmodel.h"
#include "modelregistry.h"
#include "pixeliterator.h"
#include "histogram.h"


using namespace Ilwis;
using namespace Ui;

Histogram::Histogram()
{

}

Histogram::Histogram(VisualAttribute *p) :
	VisualPropertyEditor(p, "histogrameditor", TR("Histogram"), QUrl("Histogram.qml"))
{

}

bool Histogram::canUse(const IIlwisObject &obj, const QString &name) const
{
	if (!obj.isValid())
		return false;
	if (name == VisualAttribute::LAYER_ONLY)
		return false;

	if (!hasType(obj->ilwisType(), itRASTER))
		return false;
	Ilwis::ICoverage cov = obj.as<Coverage>();

	IDomain dom = vpmodel()->domain();
	bool useable = dom.isValid() && dom->ilwisType() == itNUMERICDOMAIN;
	return !vpmodel()->layer()->isSupportLayer() && useable;
}

bool Histogram::canUse(const IIlwisObject &obj, const DataDefinition &def) const
{
	if (!hasType(obj->ilwisType(), itRASTER))
		return false;

	if (def.isValid())
		return hasType(def.domain()->ilwisType(), itNUMERICDOMAIN);
	return false;
}

VisualPropertyEditor *Histogram::create(VisualAttribute *p)
{
	return new Histogram(p);
}

void Histogram::bins2table(std::vector<NumericStatistics::HistogramBin>& hist, int columnStart) const {
	PIXVALUETYPE vstart;
	quint64 cnt = _raster->statistics(PIXELVALUE)[NumericStatistics::pCOUNT];

	//vstart = _raster->datadef().range<NumericRange>()->min();

	if (hist.size() > 0) {
		double sum = 0;

		for (int i = 0; i < hist.size() - 1; ++i) {
			auto& h = hist[i];
			sum += h._count;
			_histogramData->setCell(columnStart , i, h._count);
			double cum = 100.0 * sum / (double)cnt;
			_histogramData->setCell(columnStart + 1, i, cum);
		}
	
	}
}

std::vector<NumericStatistics::HistogramBin> Histogram::initializeBins() const{
	auto hist = _raster->statistics(PIXELVALUE).histogram();

	double rmin = _raster->statistics(PIXELVALUE).prop(NumericStatistics::pMIN);
	double rdelta = _raster->statistics(PIXELVALUE).prop(NumericStatistics::pDELTA);
	int binsize = hist.size() - 2;
	std::vector<NumericStatistics::HistogramBin> bins(binsize + 2);
	for (int i = 0; i < binsize; ++i) {
		bins[i] = NumericStatistics::HistogramBin(rmin + i * (rdelta / (binsize)));
	}
	bins[binsize] = NumericStatistics::HistogramBin(_raster->statistics(PIXELVALUE).prop(NumericStatistics::pMAX));

	return bins;
}
void Histogram::calculateLocalizedHistogram(const std::vector<Coordinate>& pol, std::vector<NumericStatistics::HistogramBin>& bins,
	NumericRange& rgnCounts, NumericRange& rngCumulatives) const {
	std::vector<geos::geom::Coordinate> *coords = new std::vector<geos::geom::Coordinate>();
	for (auto& crd : pol) {
		coords->push_back(crd);
	}
	coords->push_back(pol.front());
	geos::geom::CoordinateArraySequence *points = new  geos::geom::CoordinateArraySequence(coords);
	geos::geom::PrecisionModel *pm = new geos::geom::PrecisionModel(geos::geom::PrecisionModel::FLOATING);
	std::unique_ptr<geos::geom::GeometryFactory> geomfactory;
	geomfactory.reset(new geos::geom::GeometryFactory(pm, -1));
	geos::geom::LinearRing *ring = geomfactory->createLinearRing(points);
	UPGeometry polygon;
	polygon.reset(geomfactory->createPolygon(ring, 0));
	PixelIterator iter(_raster, polygon);
	int binsize = bins.size();
	double rmin = _raster->statistics(PIXELVALUE).prop(NumericStatistics::pMIN);
	double rdelta = _raster->statistics(PIXELVALUE).prop(NumericStatistics::pDELTA);
	quint64 cnt = _raster->statistics(PIXELVALUE)[NumericStatistics::pCOUNT];

	while (iter != iter.end()) {
		double sample = *iter;
		quint16 index = (quint16)binsize - 1;
		if (!isNumericalUndef(sample)) {
			double d = (double)(sample - rmin);
			double idx = (double)binsize * d / rdelta;
			index = idx;
			index = index >= binsize - 2 ? index - 2 : index; // -2 is the last 'real' number, -1 is the place for undefs; through rounding the index may endup at index -1 which is not what we want;
		}
		bins.at(index)._count++;
		++iter;
	}
	for (auto& h : bins) {
		rgnCounts += h._count;
	}
	double sum = 0;
	for (auto& h : bins) {
		sum += h._count;
		double cum = 100.0 * sum / (double)cnt;
		rngCumulatives += cum;
	}

}

void Histogram::addColumns(int index) {
	_histogramData->addColumn(QString("histogram_%1").arg(index), IDomain("count"));
	_histogramData->addColumn(QString("histogram_cumulative_%1").arg(index), IDomain("value"));

}

void Histogram::deleteColumns(int index) {
	_histogramData->deleteColumn(QString("histogram_%1").arg(index));
	_histogramData->deleteColumn(QString("histogram_cumulative_%1").arg(index));

}

void Histogram::updateRanges(int columnStart, const NumericRange& rngCounts, const NumericRange& rngCumulatives) {
	_histogramData->columndefinitionRef(columnStart + 2).datadef().range(rngCounts.clone()); //->as<NumericRange>()->min(vmin);
	_histogramData->columndefinitionRef(columnStart + 3).datadef().range(rngCumulatives.clone());
}

void Histogram::collectData() {
	int index = 0;
	std::vector<NumericStatistics::HistogramBin> bins;
	NumericRange rngCounts;
	NumericRange rngCumulatives;
	bins = initializeBins();
	addColumns(index);
	for (auto& pol : _polygons) {
		calculateLocalizedHistogram(pol, bins, rngCounts, rngCumulatives);
		if (_aggregateAOIs) {
			bins2table(bins, 4 + index * 2);
		}
		else {
			bins2table(bins, 4 + index * 2);
			++index;
			addColumns(index);
			bins = initializeBins(); // new bins
		}
	}
	if (_aggregateAOIs) {
		updateRanges(0, rngCounts, rngCumulatives);
	}
	else {
		for (int i = 0; i < _polygons.size(); ++i)
			updateRanges(i, rngCounts, rngCumulatives);
	}
}

QString Histogram::tableUrlPrivate()  
{
	if (!_histogramData.isValid()) {
		if (_raster.isValid()) {
			auto *layer = vpmodel()->layer();
			QVariant ccIds = layer->vproperty("colorcompositerasters");
			if (ccIds.isValid()) {
				QStringList ids = ccIds.toString().split("|");
				std::vector<std::vector<NumericStatistics::HistogramBin>> hist(3);
				getBins(ids, hist);
				_histogramData = makeCCTable(hist);
			}
			else
				_histogramData = _raster->histogramAsTable(PIXELVALUE);
		}
	}
	if ( _polygons.size() == 0 && !_useAOI)
			return  _histogramData->resource().url().toString();
	else {
		if (_useAOI) {
			collectData();
			return _histogramData->resource().url().toString();
		}
	}
	return QString();
}

void Histogram::getBins(const QStringList& ids, std::vector<std::vector<NumericStatistics::HistogramBin>>& hist) const{
	for (int i = 0; i < hist.size(); ++i) {
		quint64 id = ids[i].toULongLong();
		IRasterCoverage rasterBand;
		if (rasterBand.prepare(id)) {
			if (rasterBand->histogramCalculated(PIXELVALUE))
				hist[i] = rasterBand->statistics(PIXELVALUE).histogram();
			else {
				hist[i] = rasterBand->statistics(PIXELVALUE, ContainerStatistics<PIXVALUETYPE>::pHISTOGRAM, 0).histogram();
			}
		}
	}
}

ITable Histogram::makeCCTable(const std::vector<std::vector<NumericStatistics::HistogramBin>>& bins) const{

	auto fillCell = [&](const std::vector<NumericStatistics::HistogramBin>& bbin, int i, int columnIndex, ITable& tbl)->void {
		if (i < bbin.size()) {
			auto&  h = bbin[i];
			tbl->setCell(columnIndex, i, h._limit);
			tbl->setCell(columnIndex + 1, i, h._count);
		}
	};
	ITable histogram;

	histogram.prepare();
	histogram->addColumn("min_red", IDomain("value"), true);
	histogram->addColumn("histogram_red", IDomain("count"));
	histogram->addColumn("min_green", IDomain("value"), true);
	histogram->addColumn("histogram_green", IDomain("count"));
	histogram->addColumn("min_blue", IDomain("value"), true);
	histogram->addColumn("histogram_blue", IDomain("count"));

	for (int i = 0; i < bins[0].size() - 1; ++i) {
		fillCell(bins[0], i, 0, histogram);
		fillCell(bins[1], i, 2, histogram);
		fillCell(bins[2], i, 4, histogram);
	}

	return histogram;

}

void Histogram::prepare(const Ilwis::IIlwisObject& bj, const DataDefinition &datadef) {
	auto *layer = vpmodel()->layer();
	auto *clayer = layer->as<CoverageLayerModel>();
	IRasterCoverage raster = clayer->coverage().as<RasterCoverage>();

	_raster = raster;
	auto *lm = vpmodel()->layer()->layerManager();
	QString path = context()->ilwisFolder().absoluteFilePath();
	QUrl url = QUrl::fromLocalFile(path);
	associatedUrl(url.toString() + "/qml/datapanel/visualization/propertyeditors/PostDrawerDrawPolygons.qml");
	lm->addPostDrawer(vpmodel()->layer(), this);
}

void Histogram::addPoint(int x, int y) {

	if (_polygons.size() == 0)
		return;

	auto crd = vpmodel()->layer()->layerManager()->rootLayer()->screenGrf()->pixel2Coord(Pixel(x, y));

	Coordinate last;
	auto pol = _polygons.back();
	if (pol.size() > 2 ) {
		auto crdLast = pol.back();
		for (auto& cr : pol) {
			if (last != Coordinate()) {
				Coordinate pnt;
				if (MathHelper::lineLineIntersect(crd.x, crd.y, crdLast.x, crdLast.y, last.x, last.y, cr.x, cr.y, pnt)) {
					// the newline will always intersect with that current end of the polygons last line; which is fine as it needs to connect there
					if ( !(cr.x == pnt.x && cr.y == pnt.y && crdLast.x == pnt.x && crdLast.y == pnt.y))  
						return;
				}

			}
			last = cr;
		}
	}
	_polygons.back().push_back(crd);
	vpmodel()->layer()->layerManager()->updatePostDrawers();
}
int Histogram::polyCount() const {
	return _polygons.size();
}

QVariantList Histogram::polygon(int index) const {
	QVariantList result;
	if (index < _polygons.size()) {
		QVariantMap pnts;
		for (auto crd : _polygons[index]) {
			Pixel pix = vpmodel()->layer()->layerManager()->rootLayer()->screenGrf()->coord2Pixel(crd);
			pnts["x"] = pix.x;
			pnts["y"] = pix.y;
			result.push_back(pnts);
		}
	}
	return result;
}

void Histogram::addEmptyPolygon() {
	_polygons.resize(_polygons.size() + 1);
}

void Histogram::updateChart(const QString& sumColumn, const QString& cumColumn) {
	auto modelPair = modelregistry()->getModel(_chartModelId);
	if (modelPair.first == "chart") {
		ChartModel *chart = dynamic_cast<ChartModel *>(modelPair.second);
		if (chart) {
			if (sumColumn == "" && cumColumn == "") {
				for (int i = 0; i < _histogramData->columnCount(); ++i) {
					auto sn = QString::number(i);
					chart->deleteSerie("histogram_" + sn);
					chart->deleteSerie("histogram_cumulative_" + sn);
				}
			}
			else {
				chart->deleteSerie(sumColumn);
				chart->deleteSerie(cumColumn);
			}
		}
	}
}

void Histogram::deleteLastAOI() {
	if (_polygons.size() > 0 && _useAOI) {
		_polygons.resize(_polygons.size() - 1);
		vpmodel()->layer()->layerManager()->updatePostDrawers();
		QString sn = QString::number(_polygons.size());
		updateChart("histogram_" + sn, "histogram_cumulative_" + sn);
	}
}

void Histogram::deleteAllAOIs() {
	_polygons = std::vector<std::vector<Coordinate>>();
	vpmodel()->layer()->layerManager()->updatePostDrawers();
	updateChart();
	
}

void Histogram::useAOI(bool yesno) {
	_useAOI = yesno;
	deleteAllAOIs();
	if (_useAOI == false) {
		if (_raster.isValid()) {
			auto modelPair = modelregistry()->getModel(_chartModelId);
			if (modelPair.first == "chart") {
				ChartModel *chart = dynamic_cast<ChartModel *>(modelPair.second);
				if (chart) {
					_histogramData = _raster->histogramAsTable(PIXELVALUE);
					QString colName = "histogram|-histogram_cumulative" ;
					QString updateChart = QString("addchartdata(%1,%2,\"min\",%3,%4)").arg(_chartModelId).arg(_histogramData->resource().url(true).toString()).arg(colName).arg("\"specialtype=histogram|resx=2|resy=2\"");
					ExecutionContext ctx;
					SymbolTable symtable;
					commandhandler()->execute(updateChart, &ctx, symtable);
						//QVariantMap parms;
						//parms["raster"] = _raster->id(); 
						//chart->updateEditors(parms);
				}
			}
		}
	}
}

bool Histogram::useAOI() const {
	return _useAOI;
}

bool  Histogram::aggregateAOIs() const {
	return _aggregateAOIs;
}
void  Histogram::aggregateAOIs(bool yesno) {
	_aggregateAOIs = yesno;
}

void Histogram::chartModelId(quint32 id) {
	_chartModelId = id;
	linkChart2Editor();
	emit chartModelIdChanged();
}

quint32 Histogram::chartModelId() const {
	return _chartModelId;
}

void Histogram::linkChart2Editor() {
	auto modelPair = modelregistry()->getModel(_chartModelId);
	if (modelPair.first == "chart") {
		ChartModel *chart = dynamic_cast<ChartModel *>(modelPair.second);
		if (chart) {
			connect(chart, &ChartModel::linkSendMessage, this, &Histogram::linkAcceptMessage);
		}
	}
}

void Histogram::linkAcceptMessage(const QVariantMap& parameters) {
	if (parameters.contains("chartcloses")) {
		quint32 mid = parameters["chartcloses"].toUInt();
		if (mid == _chartModelId) {
			_chartModelId = 10000000;
			emit chartModelIdChanged();

		}
	}
}
void Histogram::updateChart(int mx, int my) {
	try {
		auto modelPair = modelregistry()->getModel(_chartModelId);
		if (modelPair.first == "chart") {
			ChartModel *chart = dynamic_cast<ChartModel *>(modelPair.second);
			if (chart) {
				Coordinate crd = vpmodel()->layer()->layerManager()->rootLayer()->screenGrf()->pixel2Coord(Pixel(mx, my));
				QVariantMap values = vpmodel()->layer()->coord2value(crd, PIXELVALUE).toMap();
				QString v;
				if (values.size() == 1) {
					v = values.first().toString();
				}
				else if ( values.size() == 3){
					v = values["red"].toString() + "|" + values["green"].toString() + "|" + values["blue"].toString();
				}

				QVariantMap parms;
				parms["editor"] = "histogramselection";
				parms["x"] = mx;
				parms["y"] = my;
				parms["value"] = v;
				chart->updateEditors(parms);
			}
		}
	}
	catch (ErrorObject& err) {}
}

void Histogram::updateAOIs() {
	try {
		auto modelPair = modelregistry()->getModel(_chartModelId);
		if (modelPair.first == "chart") {
			ChartModel *chart = dynamic_cast<ChartModel *>(modelPair.second);
			if (chart) {
				collectData();
				chart->deleteSerie("histogram");
				chart->deleteSerie("histogram_cumulative");
				chart->setMaxYLeft(rUNDEF); // forces to reset the min/max
				chart->setMaxYRight(rUNDEF);
				for (int i = 4; i < _histogramData->columnCount(); ++i) {
					QString colName = _histogramData->columndefinitionRef(i).name();
					chart->deleteSerie(colName);
					if (colName.indexOf("cumulative") != -1)
						colName = "-" + colName;
					QString updateChart = QString("addchartdata(%1,%2,\"min\",%3,%4").arg(_chartModelId).arg(_histogramData->resource().url(true).toString()).arg(colName).arg("\"specialtype=histogram|resx=2|resy=2");;
					if ( _useAOI) 
						updateChart += "|aoi=true";

					updateChart += "\")";
					ExecutionContext ctx;
					SymbolTable symtable;
					commandhandler()->execute(updateChart, &ctx, symtable);
					QVariantMap parms;
					parms["raster"] = _raster->id(); chart->updateEditors(parms);
				}
			}
		}
	}
	catch (ErrorObject& err) {}
}

bool Histogram::editState() const {
	return _editState;
}
void Histogram::editState(bool yesno) {
	_editState = yesno;
	emit editStateChanged();
}

bool Histogram::isColorComposite() const {
	auto *layer = vpmodel()->layer();
	if (layer) {
		QVariant v = layer->vproperty("colorcompositerasters");
		return v.isValid();
	}
	return false;
}
