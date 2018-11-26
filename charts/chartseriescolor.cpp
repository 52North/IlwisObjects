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
#include "chartseriescolor.h"

using namespace Ilwis;
using namespace Ui; 

REGISTER_OPERATION(ChartSeriesColor)

ChartSeriesColor::ChartSeriesColor()
{

}

ChartSeriesColor::ChartSeriesColor(quint64 metaid, const Ilwis::OperationExpression &expr) : ChartOperation(metaid, expr)
{

}

Ilwis::OperationImplementation *ChartSeriesColor::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new ChartSeriesColor(metaid, expr);
}

Ilwis::OperationImplementation::State ChartSeriesColor::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	ChartOperation::prepare(ctx, st);

	if (!getModel()) {
		kernel()->issues()->log(TR("Invalid model id used for chart. Chart can not assign new color"));
		return sPREPAREFAILED;
	}

	_seriesName = _expression.input<QString>(1);
	_newColor = _expression.input<QString>(2);

	return sPREPARED;
}

bool ChartSeriesColor::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	auto serie = _chartmodel->getSeriesByName(_seriesName);
	serie->setColor(_newColor);
    _chartmodel->updateSeriesChanged();


	return true;
}

quint64 ChartSeriesColor::createMetadata()
{
	OperationResource resource({ "ilwis://operations/chartseriescolor"  });
	resource.setLongName("Change color of a chart series");
	resource.setSyntax("chartseriescolor(parameters))");
	resource.setInParameterCount({ 3 });
	resource.addInParameter(0, itINT32, TR("chart ID"), TR("the ID identifying the chart containing the series."));
	resource.addInParameter(1, itSTRING, TR("series name"), TR(""));
	resource.addInParameter(2, itSTRING, TR("color"), TR("either an svg color name or a hex based color code (e.g #ff1145)"));
	resource.setKeywords("chart series, table, chart, color");

	mastercatalog()->addItems({ resource });
	return resource.id();
}
