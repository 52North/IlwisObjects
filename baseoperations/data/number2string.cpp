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
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "ilwisoperation.h"
#include "number2string.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(Number2String)

Number2String::Number2String()
{
}

Number2String::Number2String(quint64 metaid, const Ilwis::OperationExpression &expr) :
	OperationImplementation(metaid, expr)
{
}

bool Number2String::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	logOperation(_expression);
	ctx->setOutput(symTable, _value, sUNDEF, itSTRING, Resource());
	return true;
}

Ilwis::OperationImplementation *Number2String::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new Number2String(metaid, expr);
}

Ilwis::OperationImplementation::State Number2String::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	OperationImplementation::prepare(ctx, st);

	std::vector<QString> names{ "workingcatalog" };
	_value = _expression.parm(0).value().toLower().trimmed();


	return sPREPARED;
}

quint64 Number2String::createMetadata()
{

	OperationResource operation({ "ilwis://operations/number2string" });
	operation.setSyntax("number2string(number)");
	operation.setDescription(TR("returns string representation of the number)"));
	operation.setInParameterCount({ 1 });
	operation.addInParameter(0, itNUMBER, TR("Number"), TR("Input number"));
	operation.setOutParameterCount({ 1 });
	operation.addOutParameter(0, itSTRING, TR("string value"));
	operation.setKeywords("workflow");

	mastercatalog()->addItems({ operation });
	return operation.id();

}

