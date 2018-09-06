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
    OperationHelper::check([&]()->bool { return _inputTable.prepare(_expression.input<QString>(1), itTABLE); },
    { ERR_COULD_NOT_LOAD_2,_expression.input<QString>(1), "" });


	OperationHelper::check(
		[&]()->bool { return _chartmodel->isValidSeries(_inputTable, _expression.input<QString>(2)); },
		{ ERR_ILLEGALE_OPERATION2, _expression.input<QString>(2), "chart" }
	);

	OperationHelper::check(
		[&]()->bool { return _chartmodel->isValidSeries(_inputTable, _expression.input<QString>(3)); },
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

    QColor clr = _chartmodel->newColor();
	_chartmodel->insertDataSeries(_inputTable, _chartmodel->seriesCount(), _columnX, _columnY, sUNDEF, clr);

	logOperation(_expression);

	return true;
}

Ilwis::OperationImplementation *AddChartSeries::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new AddChartSeries(metaid, expr);
}

quint64 AddChartSeries::createMetadata()
{
	OperationResource resource({ "ilwis://operations/addchartdata" });
	resource.setLongName("Add Chart Data Series");
	resource.setSyntax("AddChartSeries(Chart ID, X Column, Y Column))");
	resource.setInParameterCount({ 4 });
	resource.addInParameter(0, itINT32, TR("Chart ID"), TR("The ID identifying the chart to which the series needs to be added. "));
    resource.addInParameter(1, itTABLE, TR("Input table"), TR("Table from which the data comes"));
	resource.addInParameter(2, itSTRING, TR("X Column"), TR("The column to use for the X-axis"));
	resource.addInParameter(3, itSTRING, TR("Y Column"), TR("The column to use for the Y-axis"));
	resource.setKeywords("chart series, table, chart");

	mastercatalog()->addItems({ resource });
	return resource.id();
}
