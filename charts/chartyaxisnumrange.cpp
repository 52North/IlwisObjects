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
#include "chartyaxisnumrange.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(ChartYAxisNumrange)

ChartYAxisNumrange::ChartYAxisNumrange()
{

}

ChartYAxisNumrange::ChartYAxisNumrange(quint64 metaid, const Ilwis::OperationExpression &expr) : ChartOperation(metaid, expr)
{

}

Ilwis::OperationImplementation *ChartYAxisNumrange::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ChartYAxisNumrange(metaid, expr);
}

Ilwis::OperationImplementation::State ChartYAxisNumrange::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    ChartOperation::prepare(ctx, st);

    if (!getModel()) {
        kernel()->issues()->log(TR("Invalid model id used for chart. Chart can not assign new Y-axis range"));
        return sPREPAREFAILED;
    }

    bool ok;
    _low = _expression.input<double>(1, ok);
    if (!ok) {
        kernel()->issues()->log(QString(TR("%1 is not a valid number").arg(_expression.parm(1).value())));
        return sPREPAREFAILED;
    }
    _high = _expression.input<double>(2, ok);
    if (!ok) {
        kernel()->issues()->log(QString(TR("%1 is not a valid number").arg(_expression.parm(2).value())));
        return sPREPAREFAILED;
    }

    _fixed = _expression.input<bool>(3, ok);
    if (!ok) {
        kernel()->issues()->log(QString(TR("%1 is not a boolean").arg(_expression.parm(3).value())));
        return sPREPAREFAILED;
    }

    return sPREPARED;
}

bool ChartYAxisNumrange::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    _chartmodel->setMinY(_low);
    _chartmodel->setMaxY(_high);
    _chartmodel->setFixedYAxis(_fixed);

    return true;
}

quint64 ChartYAxisNumrange::createMetadata()
{
    OperationResource resource({ "ilwis://operations/chartyaxisnumrange" });
    resource.setLongName("Change the Y-axis range of the chart");
    resource.setSyntax("chartyaxisnumrange(parameters)");
    resource.setInParameterCount({ 4 });
    resource.addInParameter(0, itINT32, TR("chart ID"), TR("the ID identifying the chart."));
    resource.addInParameter(1, itNUMBER, TR("low"), TR("the new lowest value for the Y-axis (specify ? to leave unchanged)"));
    resource.addInParameter(2, itNUMBER, TR("high"), TR("the new highest value for the Y-axis (specify ? to leave unchanged)"));
    resource.addInParameter(3, itBOOL, TR("fixed"), TR("fix the range to the min and max values"));
    resource.setKeywords("chart series, table, chart, Y-axis, range");

    mastercatalog()->addItems({ resource });
    return resource.id();
}
