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
#include "datadefinition.h"   
#include "columndefinition.h"         
#include "table.h"
#include "models/tablemodel.h"      
#include "chartoperationeditor.h"         
#include "symboltable.h"
#include "operationmetadata.h"   
#include "chartoperationeditor.h"          
#include "symboltable.h"
#include "operationmetadata.h"   
#include "commandhandler.h"
#include "operationExpression.h"   
#include "modelregistry.h"        
#include "operation.h"
#include "chartmodel.h" 
#include "raster.h"
#include "coveragedisplay\layermodel.h"  
#include "coveragedisplay\coveragelayermodel.h" 
#include "coveragedisplay\layermanager.h"
#include "coveragedisplay\visualattribute.h"    
#include "histogramstretchcc.h"              

using namespace Ilwis;
using namespace Ui; 

REGISTER_CHARTPROPERTYEDITOR("HistogramStretchCC", HistogramStretchCC) 

HistogramStretchCC::HistogramStretchCC() : ChartOperationEditor("HistogramStretchCC", TR("Histogram Stretch CC"), QUrl("HistogramStretchCC.qml"), TR(""))
{
}

bool HistogramStretchCC::canUse(ChartModel *model, const QVariantMap &parameter) const
{
	if (!parameter.empty())
		if (parameter["dataseries"].toBool()) {
			if (parameter.contains("specialtype")) {
				QString attr = parameter["attribute"].toString();
				bool ok = attr == "histogram_red" || attr == "histogram_green" || attr == "histogram_blue";
				const_cast<HistogramStretchCC *>(this)->_band = attr;
				return parameter["specialtype"] == "histogram" && ok;  
			}
		}
	return false;
}

QObject *HistogramStretchCC::view() {
	return chartModel()->view();
}

void HistogramStretchCC::execute(const QVariantMap &parameters)
{
	try {

	}
	catch (const ErrorObject&) {  

	}
}

double HistogramStretchCC::min() const {
	return vpmodel()->actualRange().min(); 
}

void HistogramStretchCC::prepare(ChartModel *model) {
	ChartOperationEditor::prepare(model);
	auto modelPair = modelregistry()->getModel(model->linkedModelId());
	if (modelPair.first == "rastercoverage") {
		CoverageLayerModel *rasterLayer = dynamic_cast<CoverageLayerModel *>(modelPair.second);
		if ( _band == "histogram_red")
			_rasterLayer = rasterLayer->findLayerByName("Red Layer " + rasterLayer->coverage()->name());
		else if (_band == "histogram_green")
			_rasterLayer = rasterLayer->findLayerByName("Green Layer " + rasterLayer->coverage()->name());
		else if (_band == "histogram_blue")
			_rasterLayer = rasterLayer->findLayerByName("Blue Layer " + rasterLayer->coverage()->name());
	}
}

void HistogramStretchCC::min(double value) {
	auto  range = vpmodel()->stretchRange();
	range.min(value);
	vpmodel()->stretchRange(range);
	_rasterLayer->prepare(LayerModel::ptRENDER);
	_rasterLayer->redraw();
}

double HistogramStretchCC::initStretchMin() const {
	auto  range = vpmodel()->stretchRange();
	return range.min();
}
double HistogramStretchCC::initStretchMax() const {
	auto  range = vpmodel()->stretchRange();
	return range.max();
}

double HistogramStretchCC::max() const {
	return vpmodel()->actualRange().max();
}

void HistogramStretchCC::max(double value) const {
	auto  range = vpmodel()->stretchRange();
	range.max(value);
	vpmodel()->stretchRange(range);
	_rasterLayer->prepare(LayerModel::ptRENDER);  
	_rasterLayer->redraw();
}

int HistogramStretchCC::resolution() const {
	auto range = vpmodel()->actualRange();
	if (range.resolution() == 0)
		return 2;

	if (std::floor(range.resolution()) - range.resolution() == 0)
		return 0;

	int n = std::abs(std::log10(range.resolution()));

	return n;
}

QString HistogramStretchCC::band() const {
	return _band;
}

void HistogramStretchCC::setMarkers(const QVariantList& marks) {
	if (marks.size() == 2) {
		bool ok;
		double minv = marks[0].toDouble(&ok);
		if (!ok)
			return;
		double maxv = marks[1].toDouble(&ok);
		if (!ok)
			return;
		if (minv >= maxv)
			return;

		auto rng = vpmodel()->actualRange();
		rng.min(minv);
		rng.max(maxv);
		vpmodel()->stretchRange(rng);
		_rasterLayer->prepare(LayerModel::ptRENDER);
		_rasterLayer->redraw();
	}
}

void HistogramStretchCC::markersConfirmed() {
	_rasterLayer->layerManager()->needUpdate(true); // get prepare(ptGEOMETRY) be called at the next redraw
	_rasterLayer->redraw();
}

void HistogramStretchCC::setStretchLimit(const QString& attribute, double perc) {
	ICoverage cov = coverage();
	IRasterCoverage raster = cov.as<RasterCoverage>();
	if (raster.isValid()) {

		auto limits = raster->statistics(attribute).calcStretchRange(perc);
		if (limits.first != rUNDEF && limits.second != rUNDEF)
			setMarkers({ limits.first, limits.second });
	}

}

VisualAttribute * HistogramStretchCC::vpmodel() const {
	return _rasterLayer->visualAttribute(PIXELVALUE);
}
VisualAttribute *HistogramStretchCC::vpmodel() {
	return _rasterLayer->visualAttribute(PIXELVALUE);
}


ICoverage HistogramStretchCC::coverage() const
{
	if (_rasterLayer ) {
		CoverageLayerModel *coverageLayer = static_cast<CoverageLayerModel *>(_rasterLayer);
		return coverageLayer->coverage();
	}
	return ICoverage();
}


