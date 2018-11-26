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
#include "dataseriesmodel.h"
#include "chartoperation.h"
#include "setLegendProperties.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_CHARTPROPERTYEDITOR("setLegendProperties", setLegendProperties)

setLegendProperties::setLegendProperties() : ChartOperationEditor("setlegendproperties", TR("Legend Properties"), QUrl("SetLegendProperties.qml"), TR("Adjust the legend properties"))
{
}


bool setLegendProperties::canUse(ChartModel *model, const QVariantMap &parameter) const
{
    if (!parameter.empty())
        return parameter["chart"].toBool();
    return false;
}

void setLegendProperties::execute(const QVariantMap &parameters)
{
    try {
        QString visible = parameters["show"].toString();
        QString align = parameters["align"].toString();

        quint32 cid = chartModel()->modelId();
        QString expr = QString("chartlegendproperties(%1,\"%2\",\"%3\")").arg(cid).arg(visible).arg(align);

        Ilwis::OperationExpression ex(expr);
        Ilwis::Operation op(ex);
        ExecutionContext ctx;
        SymbolTable tbl;
        op->execute(&ctx, tbl);
    }
    catch (const ErrorObject&) {

    }
}
