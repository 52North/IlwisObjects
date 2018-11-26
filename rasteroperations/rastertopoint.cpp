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
#include "featurecoverage.h"
#include "feature.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "rastertopoint.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(RasterToPoint);

RasterToPoint::RasterToPoint()
{

}

RasterToPoint::~RasterToPoint()
{

}

RasterToPoint::RasterToPoint(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid,expr)
{

}


bool RasterToPoint::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;


    QVariant value;
    value.setValue<IFeatureCoverage>(_outputfeatures);
    ctx->setOutput(symTable,value,_outputfeatures->name(), itFEATURE, _outputfeatures->source() );
    return true;
}

Ilwis::OperationImplementation *RasterToPoint::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RasterToPoint(metaid,expr);
}

Ilwis::OperationImplementation::State RasterToPoint::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    return sPREPAREFAILED;
}

quint64 RasterToPoint::createMetadata()
{
    OperationResource operation({"ilwis://operations/raster2point"});
    operation.setLongName("Transform a rastermap to a pointmap");
    operation.setKeywords("raster,pointmap");

    /*
     * more metadata
    */

    mastercatalog()->addItems({operation});
    return operation.id();
}
