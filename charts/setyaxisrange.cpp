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
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "domain.h"
#include "table.h"
#include "commandhandler.h"
#include "operation.h"
#include "mastercatalog.h"
#include "operationhelper.h"
#include "uicontextmodel.h"
#include "modelregistry.h"
#include "chartmodel.h"
#include "chartoperation.h"
#include "setyaxisrange.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_CHARTPROPERTYEDITOR("setyaxisrange", SetYAxisRange)

SetYAxisRange::SetYAxisRange() : ChartOperationEditor("setyaxisrange", TR("Set Y-axis range"), QUrl("SetYAxisRange.qml"), TR("Changes Y-axis range of the graph"))
{
}


bool SetYAxisRange::canUse(ChartModel *model, const QVariantMap &parameter) const
{
    bool ok = false;
    if (!parameter.empty()) {
        auto iter = parameter.find("yaxistype");
        if (iter != parameter.end()) {
            auto at = static_cast<ChartModel::AxisType>(iter.value().toInt());
            ok = at == ChartModel::AxisType::AT_VALUE;
        }
        return ok && parameter["dataseries"].toBool(); 
    }
    return true;
}

void SetYAxisRange::execute(const QVariantMap &parameters)
{
    try {
        double low = parameters["low"].toDouble();
        double high = parameters["high"].toDouble();
        bool fixed = parameters["fixed"].toBool();
        bool niceNumbers = parameters["nice"].toBool();
        QString fixedString = fixed ? "true" : "false";
        QString niceString = niceNumbers ? "true" : "false";
		QString useLeft = parameters["useleft"].toBool() ? "true" : "false";

        quint32 cid = chartModel()->modelId();
        QString expr = QString("chartyaxisnumrange(%1,%6,\"%2\",\"%3\",\"%4\",\"%5\")").arg(cid).arg(low).arg(high).arg(fixedString).arg(niceString).arg(useLeft);

        Ilwis::OperationExpression ex(expr);
        Ilwis::Operation op(ex);
        ExecutionContext ctx;
        SymbolTable tbl;
        op->execute(&ctx, tbl);
    }
    catch (const ErrorObject&) {

    }
}
