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

#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "fpixel.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(FPixel)

FPixel::FPixel()
{
}

FPixel::FPixel(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool FPixel::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    if ( !_pixel.isValid())
        return false;

    QVariant value;
    value.setValue<Pixel>(_pixel);
    ctx->setOutput(symTable, value, sUNDEF, itPIXEL, Resource());
    return true;
}

Ilwis::OperationImplementation *FPixel::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new FPixel(metaid, expr);
}

Ilwis::OperationImplementation::State FPixel::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    bool ok;
    double sx = _expression.parm(0).value().toLong(&ok);
    if (!ok){
        ERROR2(ERR_ILLEGAL_VALUE_2,"pixel","x");
        return sPREPAREFAILED;
    }
    double sy = _expression.parm(0).value().toLong(&ok);
    if (!ok){
        ERROR2(ERR_ILLEGAL_VALUE_2,"pixel","y");
        return sPREPAREFAILED;
    }
    double sz = 0;
    if (_expression.parameterCount() == 3) {
        sz = _expression.parm(0).value().toLong(&ok);
        if (!ok){
            ERROR2(ERR_ILLEGAL_VALUE_2,"pixel","x");
            return sPREPAREFAILED;
        }
    }
    _pixel = Pixel(sx,sy,sz);

    return sPREPARED;
}

quint64 FPixel::createMetadata()
{
    OperationResource operation({"ilwis://operations/pixel"});
    operation.setSyntax("pixel(x,y[,z])");
    operation.setDescription(TR("translates a x and y value (optional z) into a single pixel object"));
    operation.setInParameterCount({2,3});
    operation.addInParameter(0,itNUMBER, TR("x"));
    operation.addInParameter(1,itNUMBER, TR("y"));
    operation.addOptionalInParameter(2,itNUMBER, TR("z"), TR("optional"));

    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itPIXEL, TR("Pixel"));
    operation.setKeywords("pixel, geometry,workflow");

    mastercatalog()->addItems({operation});
    return operation.id();
}

