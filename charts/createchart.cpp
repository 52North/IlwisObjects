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
#include "createchart.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(CreateChart)

CreateChart::CreateChart()
{
}

CreateChart::CreateChart(quint64 metaid, const Ilwis::OperationExpression &expr) : ChartOperation(metaid, expr)
{

}

bool CreateChart::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    quint32 modelid = _chartmodel->createChart(_name, _table, _chartType, _xaxis, _yaxis, _zaxis);

    logOperation(_expression);
    ctx->setOutput(symTable, modelid, sUNDEF, itNUMBER, Resource());

    return true;
}

Ilwis::OperationImplementation *CreateChart::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateChart(metaid, expr);
}

Ilwis::OperationImplementation::State CreateChart::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    auto CheckAxis = [](const ITable& tbl, const QString& name, ChartModel::Axis atype)->quint32 {
        if (atype == ChartModel::aZAXIS && (name == sUNDEF || name == ""))
            return iUNDEF;

        quint32 index = tbl->columnIndex(name);
        if (index == iUNDEF) {
            return iUNDEF;
        }
        return index;
    };
    if (!getModel()) {
        kernel()->issues()->log(TR("Invalid model id used for chart. Chart can not be added"));
        return sPREPAREFAILED;
    }
    _name = _expression.input<QString>(1);
    OperationHelper::check([&]()->bool { return _table.prepare(_expression.input<QString>(2), itTABLE); },
    { ERR_NO_OBJECT_TYPE_FOR_2,"table", _expression.input<QString>(1)});

    QString chartType = _expression.input<QString>(3).toLower();
    QStringList types = QString("line|spline|bar|pie|points|polar|3dline|3dspline|3dbar").split("|");
    if (!types.contains(chartType)) {
        kernel()->issues()->log(TR("Unknown chart type:") + chartType);
        return sPREPAREFAILED;
    }
    _chartType = chartType;

    OperationHelper::check([&]()->bool { return CheckAxis(_table, _expression.input<QString>(4), ChartModel::aXAXIS) != iUNDEF; },
    { ERR_NO_OBJECT_TYPE_FOR_2,"column", _expression.input<QString>(4) });

    OperationHelper::check([&]()->bool { return CheckAxis(_table, _expression.input<QString>(5), ChartModel::aXAXIS) != iUNDEF; },
    { ERR_NO_OBJECT_TYPE_FOR_2,"column", _expression.input<QString>(5) });

    _zaxis = CheckAxis(_table, _expression.input<QString>(6), ChartModel::aXAXIS);

     return sPREPARED;
}

quint64 CreateChart::createMetadata()
{
    OperationResource operation({ "ilwis://operations/createchart" });
    operation.setSyntax("createchart(modelid, table, charttype=[Line|Spline|Bar|Pie|Points|Polar|3DLine|3DSpline|3Dbar], xaxis,yaxis,zaxis)");
    operation.setDescription(TR("creates a chart of the specified type"));
    operation.setInParameterCount({ 7 });
    operation.addInParameter(0, itINTEGER, TR("Chart model id"), TR("id of themodel to which this series has to be added"));
    operation.addInParameter(1, itSTRING, TR("Chart name"), TR("name of the chart"));
    operation.addInParameter(2, itTABLE, TR("Table"), TR("Table from which the series will be collected."));
    operation.addInParameter(3, itSTRING, TR("Chart type"), TR("The type of graph"));
    operation.addInParameter(4, itSTRING, TR("Name X axis"), TR("Name of the column used for the xaxis"));
    operation.addInParameter(5, itSTRING, TR("Name Y axis"), TR("Name of the column used for the yaxis")); 
    operation.addInParameter(6, itSTRING, TR("Name Z axis"), TR("Name of the column used for the yaxis. Undefined in case we have 2 dim graph"));
    operation.setOutParameterCount({ 0 });
    operation.setKeywords("table,visualization,chart");

    mastercatalog()->addItems({ operation });
    return operation.id();
}





