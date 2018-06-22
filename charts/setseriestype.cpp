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

SetSeriesType::SetSeriesType() : ChartOperationEditor("setseriestype", QUrl("SetChartType.qml"), TR("Changes the graph type of the chart"))
{
}


bool SetSeriesType::canUse(ChartModel *model, const QVariantMap &parameter) const
{
    return true;
}

void SetSeriesType::execute(const QVariantMap &parameters)
{
    try {
        QString seriesName = parameters["seriesname"].toString();
        QString color = parameters["color"].toString();
        quint32 cid = chartModel()->modelId();
        QString expr = QString("chartdataseriestype(%1,\"%2\",\"%3\")").arg(cid).arg(seriesName).arg(color);

        Ilwis::OperationExpression ex(expr);
        Ilwis::Operation op(ex);
        ExecutionContext ctx;
        SymbolTable tbl;
        if (op->execute(&ctx, tbl)) {
            chartModel()->updateSeriesChanged();
        }
    }
    catch (const ErrorObject&) {

    }
}
