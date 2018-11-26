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
#include "gridsize.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(GridSize)

GridSize::GridSize()
{
}

GridSize::GridSize(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool GridSize::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    Size<> sz = _inputGC->size();
    int dimsize = 0;
    if ( _dim == "xsize")
        dimsize =  sz.xsize();
    else if ( _dim == "ysize")
        dimsize =  sz.ysize();
    else if ( _dim == "zsize")
        dimsize = sz.zsize();
    QVariant var = dimsize;
    ctx->setOutput(symTable,var,sUNDEF,itINT32, Resource());

    return true;
}

OperationImplementation *GridSize::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new GridSize(metaid, expr);
}

OperationImplementation::State GridSize::prepare(ExecutionContext *ctx, const SymbolTable &st ){
    OperationImplementation::prepare(ctx,st);
    QString raster = _expression.parm(0).value();
    if (!_inputGC.prepare(raster)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
     QString dim  =_expression.parm(1).value().toLower();
     if ( dim != "xsize" && dim != "ysize" && dim != "zsize") {
         ERROR2(ERR_ILLEGAL_VALUE_2,"dimension value",dim);
         return sPREPAREFAILED;
     }
     _dim = dim;

     return sPREPARED;

}

quint64 GridSize::createMetadata()
{
    OperationResource operation({"ilwis://operations/rastersize"});
    operation.setSyntax("gridsize(rastercoverage,dimension=xsize|ysize|zsize)");
    operation.setLongName("Raster Gridsize");
    operation.setDescription(TR("returns the pixel extent of a grid coverage dimension"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER, TR("rastercoverage"),TR("rastercoverage from which a dimension size is retrieved"));
    operation.addInParameter(1,itSTRING, TR("dimension"));

    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itINT32, TR("size"),TR("Lenght in one dimension measured in grid cells") );
    operation.setKeywords("raster, size,workflow");

    mastercatalog()->addItems({operation});
    return operation.id();
}
