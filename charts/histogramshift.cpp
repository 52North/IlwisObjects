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
#include "operation.h"
#include "chartmodel.h"  
#include "dataseriesmodel.h"
#include "histogramshift.h"               

using namespace Ilwis;
using namespace Ui;

REGISTER_CHARTPROPERTYEDITOR("histogramshift", HistogramShift)   

HistogramShift::HistogramShift() : ChartOperationEditor("histogramshift", TR("Histogram Shift"), QUrl("HistogramShift.qml"), TR("Enhance linked raster by simply shifting the whole histogram"))
{
}

bool HistogramShift::canUse(ChartModel *model, const QVariantMap &parameter) const 
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

void HistogramShift::execute(const QVariantMap &parameters)  
{
	try {

	}
	catch (const ErrorObject&) {

	}
}

double HistogramShift::minValue() const {
	return chartModel()->minx();
}
double HistogramShift::maxValue() const {
	return chartModel()->maxx();

}

void HistogramShift::addIntensityCurve() {
	DataseriesModel *dm = chartModel()->getSeriesByName("shifted_histogram");
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
		QString updateChart = QString("addchartdata(%1,%2,\"min\",%3,%4)").arg(chartModel()->modelId()).arg(pntsTable->resource().url(true).toString()).arg("histogram").arg("\"resx=2|resy=2|name=shifted_histogram\"");
		ExecutionContext ctx;
		SymbolTable symtable;
		commandhandler()->execute(updateChart, &ctx, symtable);   
	}


}

void HistogramShift::removeIntensityCurve() { 
	chartModel()->deleteSerieByName("shifted_histogram");

}

bool HistogramShift::isActive() const {
	return chartModel()->getSeriesByName("shifted_histogram") != 0; 
}

void HistogramShift::shiftValues(double shift) {
	DataseriesModel *dm = chartModel()->getSeriesByName("shifted_histogram"); 
	if (dm) {
		if (_basePoints.size() == 0)
			_basePoints = dm->points();
		auto newPoints = _basePoints;
		for (auto& pnt : newPoints) {
			pnt.setX(pnt.x() + shift);
		}
		dm->points(newPoints);
	}
}


