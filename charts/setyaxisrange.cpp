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

        quint32 cid = chartModel()->modelId();
        QString expr = QString("chartyaxisnumrange(%1,\"%2\",\"%3\",\"%4\",\"%5\")").arg(cid).arg(low).arg(high).arg(fixedString).arg(niceString);

        Ilwis::OperationExpression ex(expr);
        Ilwis::Operation op(ex);
        ExecutionContext ctx;
        SymbolTable tbl;
        op->execute(&ctx, tbl);
    }
    catch (const ErrorObject&) {

    }
}
