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
#include "histogrambounds.h"       

using namespace Ilwis;
using namespace Ui;

REGISTER_CHARTPROPERTYEDITOR("histogrambounds", HistogramBounds)

HistogramBounds::HistogramBounds() : ChartOperationEditor("histogrambounds", TR("Histogram Bounds"), QUrl("HistogramBounds.qml"), TR("Interactive setting selections percentage based bounds on the cummulatie chart"))
{
}

bool HistogramBounds::canUse(ChartModel *model, const QVariantMap &parameter) const
{
	if (!parameter.empty())
		if (parameter["dataseries"].toBool()) {
			if (parameter.contains("specialtype")) {
				return parameter["specialtype"] == "histogram" && parameter["attribute"].toString().indexOf("histogram_cumulative") == 0;          
			}
		}
	return false;
}

int HistogramBounds::yPercValue(double v) { 
	ITable tbl = chartModel()->dataTable();
	for (int rec = 0; rec < tbl->recordCount(); ++rec) {     
		int vBin = tbl->cell(0, rec).toInt();
		if (v < vBin) {
			double l1 = tbl->cell(0, rec - 1).toInt();
			double l2 = tbl->cell(0, rec).toInt();
			double fac = (v - l1) / (l2 - l1);
			l1 = tbl->cell(2, rec - 1).toInt();
			l2 = tbl->cell(2, rec).toInt();
			int perc = l1 + (l2 - l1)*fac;
			return perc;
		}
	}
	return 0;
}

void HistogramBounds::execute(const QVariantMap &parameters)
{
	try {

	}
	catch (const ErrorObject&) {

	}
}




