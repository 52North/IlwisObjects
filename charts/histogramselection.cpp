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
#include "HistogramSelection.h"    

using namespace Ilwis;
using namespace Ui;

REGISTER_CHARTPROPERTYEDITOR("histogramselection", HistogramSelection)

HistogramSelection::HistogramSelection() : ChartOperationEditor("histogramselection", TR("Histogram Selection"), QUrl("HistogramSelection.qml"), TR("Interactive setting selections in the histogram and seeing results in the corresponding raster"))
{
}

bool HistogramSelection::canUse(ChartModel *model, const QVariantMap &parameter) const
{
	if (!parameter.empty())
		if (parameter["dataseries"].toBool()) {
			if (parameter.contains("specialtype")) { 
				return parameter["specialtype"] == "histogram"; 
			}
		}
	return false;
}

void HistogramSelection::execute(const QVariantMap &parameters)
{
	try {

	}
	catch (const ErrorObject&) {

	}
}


