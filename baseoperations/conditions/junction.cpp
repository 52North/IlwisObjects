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
#include "coverage.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "columndefinition.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "raster.h"
#include "table.h"
#include "feature.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "operation.h"
#include "junction.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(Junction)

Ilwis::OperationImplementation *Junction::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Junction(metaid, expr);
}

Junction::Junction()
{
}

Junction::Junction(quint64 metaid,const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool Junction::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    QVariant variant;
    QString setValue = _firstValue != sUNDEF ? _firstValue : _secondValue;


    // TODO: setvalue is the name. This should be the object itself.
    if (_type == "RasterCoverage")
        variant.setValue<IRasterCoverage>(setValue);
    else if (_type == "FeatureCoverage")
        variant.setValue<IFeatureCoverage>(setValue);
    else if (_type == "Table")
        variant.setValue<ITable>(setValue);
    else
        variant.setValue<QString>(setValue);

    // TODO: actually send type
    ctx->addOutput(symTable, variant, sUNDEF, 0, Resource());
    return true;
}

OperationImplementation::State Junction::prepare(ExecutionContext *ctx,const SymbolTable&)
{
    if ( _expression.parameterCount() != 3)
        return sPREPAREFAILED;

    _firstValue = _expression.parm(0).value();
    _secondValue = _expression.parm(1).value();

    _type = _expression.parm(2).value();

    return sPREPARED;
}


quint64 Junction::createMetadata()
{
    OperationResource operation({"ilwis://operations/junction"});
    operation.setLongName("Junction function");
    operation.setSyntax("junction(FirstValue, SecondValue, Type=!RasterCoverage|FeatureCoverage|Number|String|Table|Boolean)");
    operation.setDescription(TR("Returns set value. This operation is only usefull in the modeler. Espacially when using conditions. Value is not set if it is empty"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itANY, TR("first value"),TR("returns this if set"));
    operation.addInParameter(1,itANY, TR("second value"),TR("returns this if first value is not set"));
    operation.addInParameter(2,itSTRING, TR("type"),TR("The type of the values"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itANY, TR("set value"), TR("first if set else second"));
    operation.setKeywords("junction,workflow");

    mastercatalog()->addItems({operation});
    return operation.id();
}
