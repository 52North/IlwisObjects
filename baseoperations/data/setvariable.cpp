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
#include "commandhandler.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "operationhelper.h"
#include "setvariable.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(SetVariable)

SetVariable::SetVariable()
{
}

SetVariable::SetVariable(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

bool SetVariable::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    QVariant v(_value);

    logOperation( _expression);
    ctx->setOutput(symTable, v, "var",itSTRING, Resource());
    return true;
}

Ilwis::OperationImplementation *SetVariable::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
        return new SetVariable(metaid, expr);
}

Ilwis::OperationImplementation::State SetVariable::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    _value = _expression.input<QString>(0);

    return sPREPARED;
}

quint64 SetVariable::createMetadata()
{

    OperationResource operation({"ilwis://operations/setvariable"});
    operation.setSyntax("setvariable(value)");
    operation.setDescription(TR("creates a variable that can be used in worflows"));
    operation.setInParameterCount({1});
    operation.addInParameter(0,itSTRING , TR("value"),TR("Value of the variable"));
    operation.parameterNeedsQuotes(0);
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itANY , TR("value"),TR(""));
    operation.setKeywords("workflow");

    mastercatalog()->addItems({operation});
    return operation.id();

}


