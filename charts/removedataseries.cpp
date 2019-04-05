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
#include "removedataseries.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_CHARTPROPERTYEDITOR("removedataseries", RemoveDataSeries)

RemoveDataSeries::RemoveDataSeries() : ChartOperationEditor("removedataseries", TR("Remove Data serie"), QUrl("RemoveDataSeries.qml"), TR("remove the dataserie from the chart"))
{
}

bool RemoveDataSeries::canUse(ChartModel *model, const QVariantMap &parameter) const
{
	if (!parameter.empty())
		return parameter["dataseries"].toBool();
	return false;
}

void RemoveDataSeries::execute(const QVariantMap &parameters)
{
	try { 
		QString seriesName = parameters["seriesname"].toString();
		quint32 cid = chartModel()->modelId();
		QString expr = QString("deletechartseries(%1,\"%2\")").arg(cid).arg(seriesName);

		Ilwis::OperationExpression ex(expr);
		Ilwis::Operation op(ex);
		ExecutionContext ctx;
		SymbolTable tbl;
		op->execute(&ctx, tbl);
	}
	catch (const ErrorObject&) {

	}
}
