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
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "operation.h"
#include "iscompatible.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(IsCompatible)


Ilwis::OperationImplementation *IsCompatible::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new IsCompatible(metaid, expr);
}

IsCompatible::IsCompatible()
{
}

IsCompatible::IsCompatible(quint64 metaid,const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool IsCompatible::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    bool result = _object1->isCompatibleWith(_object2.ptr());

    logOperation(_expression);
    ctx->setOutput(symTable, QVariant(result), sUNDEF, itBOOL, Resource());
    return true;
}

OperationImplementation::State IsCompatible::prepare(ExecutionContext *ctx,const SymbolTable& st)
{
    OperationImplementation::prepare(ctx,st);

    _role = TypeHelper::name2type(_expression.input<QString>(2));
    std::vector<IlwisTypes> types = {itGEOREF, itCOORDSYSTEM, itDOMAIN, itRASTER, itFEATURE, itTABLE};
    auto iter = std::find(types.begin(), types.end(), _role);
    if ( iter == types.end()){
        kernel()->issues()->log(TR("Unusable type for compatibility test: ") + _expression.input<QString>(2));
        return sPREPAREFAILED;
    }
    OperationHelper::check([&] ()->bool { return _object1.prepare(_expression.input<QString>(0), _role); },
    {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

    OperationHelper::check([&] ()->bool { return _object2.prepare(_expression.input<QString>(1), _role); },
    {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(1), "" } );

    return sPREPARED;
}


quint64 IsCompatible::createMetadata()
{
    OperationResource operation({"ilwis://operations/iscompatible"});
    operation.setSyntax("iscompatible(firstvalue,secondvalue,georeference|coordinatesystem|domain|raster|featurecoverage|table)");
    operation.setDescription(TR("Checks if two ilwisobjects are compatible with each othert. For example are two georefs compatible/equal"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itILWISOBJECT, TR("first value"),TR("left hand of the IsCompatible operation"));
    operation.addInParameter(1,itILWISOBJECT, TR("second value"),TR("right hand of the IsCompatible method"));
    operation.addInParameter(2,itSTRING,TR("object role"),TR("the typer that is represented by both urls needs to included as objects (urls) can be have multiple roles"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itBOOL, TR("result"), TR("Result of the comparison"));
    operation.setKeywords("tests, workflow");

    mastercatalog()->addItems({operation});
    return operation.id();
}

