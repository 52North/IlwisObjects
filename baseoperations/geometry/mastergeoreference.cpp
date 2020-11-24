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
#include "mastergeoreference.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(MasterGeoreference)

MasterGeoreference::MasterGeoreference()
{
}

Ilwis::OperationImplementation *MasterGeoreference::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new MasterGeoreference(metaid, expr);
}

MasterGeoreference::MasterGeoreference(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

bool MasterGeoreference::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    ctx->_masterGeoref = _georef->resource().url().toString();

    return true;
}

Ilwis::OperationImplementation::State MasterGeoreference::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
   QString georef = _expression.parm(0).value();
   if(!_georef.prepare(georef)) {
       ERROR2(ERR_COULD_NOT_LOAD_2,georef,"");
       return sPREPAREFAILED;
   }
   return sPREPARED;
}

quint64 MasterGeoreference::createMetadata()
{
    OperationResource operation({"ilwis://operations/mastergeoreference"});
    operation.setSyntax("mastergeoref(targetgeoref)");
    operation.setDescription(TR("Sets the georeference for all operations that trigger an on the fly resampling of a gridcoverage"));
    operation.setInParameterCount({1});
    operation.addInParameter(0,itRASTER|itGEOREF, TR("georeference"),TR("input georeference or raster, in which case it takes the georeference of the raster"));

    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itUNKNOWN, TR(""));
    operation.setKeywords("georeference,workflow");

    operation.checkAlternateDefinition();
    mastercatalog()->addItems({operation});
    return operation.id();
}
