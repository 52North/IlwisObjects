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

	if (!hasType(obj->ilwisType(), itCOVERAGE))
		return false;
	Ilwis::ICoverage cov = obj.as<Coverage>();

	IDomain dom = vpmodel()->domain();
	bool useable = dom.isValid() && dom->ilwisType() == itNUMERICDOMAIN;
	return !vpmodel()->layer()->isSupportLayer() && useable;
}

bool Histogram::canUse(const IIlwisObject &, const DataDefinition &def) const
{
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
			//_histogramData->setCell(columnStart, i, vstart);
			//_histogramData->setCell(columnStart + 1, i, h._limit);
			_histogramData->setCell(columnStart , i, h._count);
			double cum = 100.0 * sum / (double)cnt;
			_histogramData->setCell(columnStart + 1, i, cum);

			//vstart = h._limit;
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
	//_histogramData->addColumn(QString("min_%1").arg(index), IDomain("value"), true);
	//_histogramData->addColumn(QString("max_%1").arg(index), IDomain("value"), true);
	_histogramData->addColumn(QString("histogram_%1").arg(index), IDomain("count"));
	_histogramData->addColumn(QString("histogram_cumulative_%1").arg(index), IDomain("value"));

}

void Histogram::deleteColumns(int index) {
	//_histogramData->deleteColumn(QString("min_%1").arg(index));
	//_histogramData->deleteColumn(QString("max_%1").arg(index));
	_histogramData->deleteColumn(QString("histogram_%1").arg(index));
	_histogramData->deleteColumn(QString("histogram_cumulative_%1").arg(index));

}

void Histogram::updateRanges(int columnStart, const NumericRange& rngCounts, const NumericRange& rngCumulatives) {
	_histogramData->columndefinitionRef(columnStart + 2).datadef().range(rngCounts.clone()); //->as<NumericRange>()->min(vmin);
	_histogramData->columndefinitionRef(columnStart + 3).datadef().range(rngCumulatives.clone());
}

QString Histogram::tableUrlPrivate()  
{
	if (_histogramData.isValid()) {
		if ( _polygons.size() == 0 && !_useAOI)
			return  _histogramData->resource().url().toString();
		else {
			if (_useAOI) {
				int index = 0;
				std::vector<NumericStatistics::HistogramBin> bins;
				NumericRange rngCounts;
				NumericRange rngCumulatives;
				bins = initializeBins();
				for (auto& pol : _polygons) {
					calculateLocalizedHistogram(pol, bins, rngCounts, rngCumulatives);
					if (_aggregateAOIs) {
						bins2table(bins, 2 + index * 2);
					}
					else {
						addColumns(index);
						bins2table(bins, 4 + index * 2);
						++index;
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
				return _histogramData->resource().url().toString();
			}
		}
	}
	return QString();
}

void Histogram::prepare(const Ilwis::IIlwisObject& bj, const DataDefinition &datadef) {
	auto *layer = vpmodel()->layer();
	auto *clayer = layer->as<CoverageLayerModel>();
	IRasterCoverage raster = clayer->coverage().as<RasterCoverage>();
	if (raster.isValid()) {
		_histogramData = raster->histogramAsTable(PIXELVALUE);
	}
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

	for (auto& pol : _polygons) {
		Coordinate last;
		if (pol.size() > 2 && _polygons.back().size() > 0) {
			auto crdLast = _polygons.back().back();
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

void Histogram::deleteLastAOI() {
	if ( _polygons.size() > 0)
		_polygons.resize(_polygons.size() - 1);
	vpmodel()->layer()->layerManager()->updatePostDrawers();
}
void Histogram::deleteAllAOIs() {
	_polygons = std::vector<std::vector<Coordinate>>();
	vpmodel()->layer()->layerManager()->updatePostDrawers();
}

void Histogram::useAOI(bool yesno) {
	_useAOI = yesno;
	vpmodel()->layer()->layerManager()->updatePostDrawers();
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



