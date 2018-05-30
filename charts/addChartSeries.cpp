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
#include "AddChartSeries.h" 

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(AddChartSeries)

AddChartSeries::AddChartSeries()
{

}

AddChartSeries::AddChartSeries(quint64 metaid, const Ilwis::OperationExpression &expr) : ChartOperation(metaid, expr)
{

}

Ilwis::OperationImplementation::State AddChartSeries::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	ChartOperation::prepare(ctx, st);

	if (!getModel()) {
		kernel()->issues()->log(TR("Invalid model id used for chart. Chart can not be added"));
		return sPREPAREFAILED;
	}
	_name = _expression.input<QString>(1);

	OperationHelper::check(
		[&]()->bool { return _chartmodel->isValidSeries(_expression.input<QString>(2)); },
		{ ERR_ILLEGALE_OPERATION2, _expression.input<QString>(2), "chart" }
	);

	OperationHelper::check(
		[&]()->bool { return _chartmodel->isValidSeries(_expression.input<QString>(3)); },
		{ ERR_ILLEGALE_OPERATION2, _expression.input<QString>(3), "chart" }
	);

	_columnX = _expression.input<QString>(2);
	_columnY = _expression.input<QString>(3);

	return sPREPARED;
} 

bool AddChartSeries::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	quint32 xaxis = _table->columnIndex(_columnX);
	quint32 yaxis = _table->columnIndex(_columnY);
	quint32 zaxis = iUNDEF;
    QColor clr = _chartmodel->newColor();
	_chartmodel->addDataSeries(xaxis, yaxis, zaxis, clr);

	logOperation(_expression);

	return true;
}

Ilwis::OperationImplementation *AddChartSeries::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new AddChartSeries(metaid, expr);
}

quint64 AddChartSeries::createMetadata()
{
	OperationResource resource({ "ilwis://operations/AddChartSeries" });
	resource.setLongName("Add Chart Series");
	resource.setSyntax("AddChartSeries(Chart ID, X Column, Y Column))");
	resource.setInParameterCount({ 3 });
	resource.addInParameter(0, itINT32, TR("Chart ID"), TR("The ID identifying the chart to which the series needs to be added. "));
	resource.addInParameter(1, itSTRING, TR("X Column"), TR("The column to use for the X-axis"));
	resource.addInParameter(2, itSTRING, TR("Y Column"), TR("The column to use for the Y-axis"));
	resource.setKeywords("chart series, table, chart");

	mastercatalog()->addItems({ resource });
	return resource.id();
}
