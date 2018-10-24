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
