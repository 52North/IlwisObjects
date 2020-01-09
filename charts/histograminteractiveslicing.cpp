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
#include "histograminteractiveslicing.h"    

using namespace Ilwis;
using namespace Ui;

REGISTER_CHARTPROPERTYEDITOR("histograminteractiveslicing", HistogramInteractiveSlicing)

HistogramInteractiveSlicing::HistogramInteractiveSlicing() : ChartOperationEditor("histograminteractiveslicing", TR("Interactive Slicing"), QUrl("HistogramInteractiveSlicing.qml"), TR("Manual slicing classification by selecting parts on the histogram function as class boundaries"))
{
}

bool HistogramInteractiveSlicing::canUse(ChartModel *model, const QVariantMap &parameter) const
{
	if (!parameter.empty())
		if (parameter["dataseries"].toBool()) {
			if (parameter.contains("specialtype")) {
				return parameter["specialtype"] == "histogram";
			}
		}
	return false;
}

void HistogramInteractiveSlicing::execute(const QVariantMap &parameters)
{
	try {

	}
	catch (const ErrorObject&) {

	}
}


