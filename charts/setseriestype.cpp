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
#include "setseriestype.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_CHARTPROPERTYEDITOR("setseriestype", SetSeriesType)

SetSeriesType::SetSeriesType() : ChartOperationEditor("setdataseriestype", TR("Set Dataseries Type"), QUrl("SetChartType.qml"), TR("Changes the graph type of the dataseries"))
{
}


bool SetSeriesType::canUse(ChartModel *model, const QVariantMap &parameter) const
{
    if (!parameter.empty())
        return parameter["dataseries"].toBool();
    return false;
}

void SetSeriesType::execute(const QVariantMap &parameters)
{
    try {
        QString seriesName = parameters["seriesname"].toString();
        QString type = parameters["charttype"].toString();
        if (type.length() == 0)
            return;
        quint32 cid = chartModel()->modelId();
        QString expr = QString("chartdataseriestype(%1,\"%2\",\"%3\")").arg(cid).arg(seriesName).arg(type);

        Ilwis::OperationExpression ex(expr);
        Ilwis::Operation op(ex);
        ExecutionContext ctx;
        SymbolTable tbl;
        op->execute(&ctx, tbl);
    }
    catch (const ErrorObject&) {

    }
}
