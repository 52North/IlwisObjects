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
#include "changedataseriesname.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(ChangeDataSeriesName)

ChangeDataSeriesName::ChangeDataSeriesName()
{

}

ChangeDataSeriesName::ChangeDataSeriesName(quint64 metaid, const Ilwis::OperationExpression &expr) : ChartOperation(metaid, expr)
{

}

Ilwis::OperationImplementation::State ChangeDataSeriesName::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	ChartOperation::prepare(ctx, st);

	if (!getModel()) {
		kernel()->issues()->log(TR("Invalid model id used for chart. Chart can not be added"));
		return sPREPAREFAILED;
	}
	_oldName = _expression.input<QString>(1);
	auto *series = _chartmodel->getSeriesByName(_oldName);
	if (!series) {
		return sPREPSKIP;
	}
	_newName = _expression.input<QString>(2);
	series = _chartmodel->getSeriesByName(_newName);
	if (series) {
		return sPREPSKIP;
	}

	
	return sPREPARED;
}

bool ChangeDataSeriesName::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	if(_prepState != sPREPSKIP)
		_chartmodel->changeDataSeriesName(_oldName, _newName);

	logOperation(_expression);

	return true;
}

Ilwis::OperationImplementation *ChangeDataSeriesName::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new ChangeDataSeriesName(metaid, expr);
}

quint64 ChangeDataSeriesName::createMetadata()
{
	OperationResource resource({ "ilwis://operations/changedataseriesname" });
	resource.setLongName("Change data series name");
	resource.setSyntax("changedataseriesname(chartid, oldname, newname))");
	resource.setInParameterCount({ 3 });
	resource.addInParameter(0, itINT32, TR("Chart ID"), TR("The ID identifying the chart to which the series needs to be updated. "));
	resource.addInParameter(1, itSTRING, TR("Old Name"), TR("The column to use for the X-axis"));
	resource.addInParameter(2, itSTRING, TR("New Name"), TR("The column to use for the Y-axis"));
	resource.setKeywords("table, chart");

	mastercatalog()->addItems({ resource });
	return resource.id();
}
