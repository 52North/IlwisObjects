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
#include "setseriescolor.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_CHARTPROPERTYEDITOR("setseriescolor", SetSeriesColor)

SetSeriesColor::SetSeriesColor() : ChartOperationEditor("setseriescolor", TR("Set Series Color"), QUrl("SetSeriesColor.qml"), TR("Changes the color of the selected data series in the chart"))
{
}

bool SetSeriesColor::canUse(ChartModel *model, const QVariantMap &parameter) const
{
    if (!parameter.empty())
        return parameter["dataseries"].toBool();
    return false;
}

void SetSeriesColor::execute(const QVariantMap &parameters)
{
	try {
		QString seriesName = parameters["seriesname"].toString();
		QString color = parameters["color"].toString();
        quint32 cid = chartModel()->modelId();
        QString expr = QString("chartseriescolor(%1,\"%2\",\"%3\")").arg(cid).arg(seriesName).arg(color);

		Ilwis::OperationExpression ex(expr);
		Ilwis::Operation op(ex);
		ExecutionContext ctx;
		SymbolTable tbl;
        op->execute(&ctx, tbl);
	}
	catch (const ErrorObject&) {

	}
}
