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
#include "commandhandler.h"
#include "operationExpression.h"   
#include "modelregistry.h"
#include "operation.h"
#include "chartmodel.h" 
#include "coveragedisplay\layermodel.h" 
#include "coveragedisplay\layermanager.h"
#include "dataseriesmodel.h"
#include "continuouscolorlookup.h"
#include "histogramstretchtool.h"              

using namespace Ilwis; 
using namespace Ui;

REGISTER_CHARTPROPERTYEDITOR("histogramstretchtool", HistogramStretchTool)

HistogramStretchTool::HistogramStretchTool() : ChartOperationEditor("histogramstretchtool", TR("Histogram Stretch Tool"), QUrl("HistogramStretchTool.qml"), TR("Interactive setting the mapping of real values and their stretched counterparts"))
{
}

bool HistogramStretchTool::canUse(ChartModel *model, const QVariantMap &parameter) const 
{
	if (!parameter.empty())
		if (parameter["dataseries"].toBool()) {  
			if (parameter.contains("specialtype")) {
				QString attr = parameter["attribute"].toString();
				bool ok = attr.indexOf("histogram") == 0 && attr.size() < 12;     
				return parameter["specialtype"] == "histogram" && ok;    
			}
		}
	return false;
}

void HistogramStretchTool::prepare(ChartModel *model) {
	ChartOperationEditor::prepare(model);
	auto modelPair = modelregistry()->getModel(model->linkedModelId()); 
	if (modelPair.first == "rastercoverage") {
		_rasterLayer = dynamic_cast<LayerModel *>(modelPair.second);
		_actualRange = _rasterLayer->vproperty("actualrange").value<NumericRange>(); 
		_mapping.resize(2);
		_mapping[0].first = _mapping[0].second = _actualRange.min();
		_mapping[1].first = _mapping[1].second = _actualRange.max();    
	}
}

void HistogramStretchTool::execute(const QVariantMap &parameters) 
{
	try {

	}
	catch (const ErrorObject&) {

	}
}

double HistogramStretchTool::actualMin() const { 
	return _actualRange.min();
}
double HistogramStretchTool::actualMax() const {
	return _actualRange.max();
}

void HistogramStretchTool::addIntensityCurve() {
	DataseriesModel *dm = chartModel()->getSeriesByName("stretched_histogram");
	if (dm == 0) {
		auto pnts = chartModel()->getSeries(0)->points();
		ITable pntsTable;
		pntsTable.prepare();
		pntsTable->addColumn("min", IDomain("value"), true); 
		pntsTable->addColumn("histogram", IDomain("count"));
		int count = 0;
		for (const auto& v : pnts) {
			QPointF pnt = v;
			pntsTable->record(count++, { pnt.x(), pnt.y() }); 
		}
		QString updateChart = QString("addchartdata(%1,%2,\"min\",%3,%4)").arg(chartModel()->modelId()).arg(pntsTable->resource().url(true).toString()).arg("histogram").arg("\"resx=2|resy=2|name=stretched_histogram\"");
		ExecutionContext ctx;
		SymbolTable symtable;
		commandhandler()->execute(updateChart, &ctx, symtable);       
	}


}

void HistogramStretchTool::removeIntensityCurve() {       
}

void  HistogramStretchTool::stretchHistogram(bool isLower, double origValue, double newValue) {
	DataseriesModel *dm = chartModel()->getSeriesByName("stretched_histogram");
	if (dm) {
		if (_basePoints.size() == 0)
			_basePoints = dm->points(); 
		auto newPoints = _basePoints;
		int idx = isLower ? 0 : 1;
		_mapping[idx].first = origValue;
		_mapping[idx].second = newValue; 
		for (auto& pnt : newPoints) { 
			pnt.setX(ContinuousColorLookup::value2Color4BoundsMapping(pnt.x(), _actualRange, _mapping));
		}
		dm->points(newPoints);   
	}
}

void HistogramStretchTool::updateTextures() {
	_rasterLayer->layerManager()->needUpdate(true); 
}

QVariantMap HistogramStretchTool::markers() const { 
	QVariantMap markers;
	if (_mapping.size() == 2) {
		markers["loweroriginal"] = _mapping[0].first;
		markers["lowermapped"] = _mapping[0].second;
		markers["upperoriginal"] = _mapping[1].first;
		markers["uppermapped"] = _mapping[1].second;
	}

	return markers;
}




