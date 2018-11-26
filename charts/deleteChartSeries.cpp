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
#include "deleteChartSeries.h" 

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(deleteChartSeries)

deleteChartSeries::deleteChartSeries()
{

}

deleteChartSeries::deleteChartSeries(quint64 metaid, const Ilwis::OperationExpression &expr) : ChartOperation(metaid, expr)
{

}

Ilwis::OperationImplementation::State deleteChartSeries::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	ChartOperation::prepare(ctx, st);

	if (!getModel()) {
		kernel()->issues()->log(TR("Invalid model id used for chart. Chart can not be added"));
		return sPREPAREFAILED;
	}

	/*OperationHelper::check(
		[&]()->bool { return _chartmodel->isValidSeries(_inputTable, _expression.input<QString>(1)); },
		{ ERR_ILLEGALE_OPERATION2, _expression.input<QString>(2), "chart" }
	);*/

	_columnY = _expression.input<QString>(1);

	return sPREPARED;
} 

bool deleteChartSeries::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

    QColor clr = _chartmodel->newColor();
	_chartmodel->deleteSerie(_columnY,sUNDEF);

	logOperation(_expression);

	return true;
}

Ilwis::OperationImplementation *deleteChartSeries::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new deleteChartSeries(metaid, expr);
}

quint64 deleteChartSeries::createMetadata()
{
	OperationResource resource({ "ilwis://operations/deletechartseries" });
	resource.setLongName("Delete Chart Data Series");
	resource.setSyntax("deletechartseries(Chart ID, Y Column))");
	resource.setInParameterCount({ 2 });
	resource.addInParameter(0, itINT32, TR("Chart ID"), TR("The ID identifying the chart from which the series should be deleted. "));
	resource.addInParameter(1, itSTRING, TR("Y Column"), TR("The column that should be deleted"));
	resource.setKeywords("chart series, table, chart");

	mastercatalog()->addItems({ resource });
	return resource.id();
}
