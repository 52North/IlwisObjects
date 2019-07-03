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
#include "rasterinterpolator.h"
#include "resampleraster.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(ResampleRaster)

Ilwis::OperationImplementation *ResampleRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ResampleRaster(metaid, expr);
}

ResampleRaster::ResampleRaster()
{
}

ResampleRaster::ResampleRaster(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr),
    _method(RasterInterpolator::ipBICUBIC)
{
}

bool ResampleRaster::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    BoxedAsyncFunc resampleFun = [&](const BoundingBox& box) -> bool {
        PixelIterator iterOut(_outputRaster,box);
        RasterInterpolator interpolator(_inputRaster, _method);
        PixelIterator iterEnd = iterOut.end();
        bool equalCsy = _inputRaster->coordinateSystem()->isEqual(_outputRaster->coordinateSystem().ptr());
        while(iterOut != iterEnd) {
            Pixel position = iterOut.position();
            Coordinate coord = _outputRaster->georeference()->pixel2Coord(Pixeld(position.x,(position.y)));
            if ( !equalCsy)
                coord = _inputRaster->coordinateSystem()->coord2coord(_outputRaster->coordinateSystem(),coord);
            double vnew = interpolator.coord2value(coord, iterOut.position().z);
            *iterOut = vnew;
            ++iterOut;
            updateTranquilizer(iterOut.linearPosition(), 1000);
        }
        return true;
    };

    bool resource = OperationHelperRaster::execute(ctx, resampleFun, _outputRaster);


    if ( resource && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputRaster);
		
        ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->resource() );
    }
    return resource;
}

Ilwis::OperationImplementation::State ResampleRaster::prepare(ExecutionContext *ctx, const SymbolTable &st )
{
    OperationImplementation::prepare(ctx,st);
    QString raster = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputRaster.prepare(raster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
    OperationHelperRaster::initialize(_inputRaster,_outputRaster, itDOMAIN|itTABLE);
    if ( !_outputRaster.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }
    IGeoReference grf;
    grf.prepare(_expression.parm(1).value());
    if ( !grf.isValid()) {
        return sPREPAREFAILED;
    }
    _outputRaster->georeference(grf);
    Size<> sz = grf->size();
    if ( sz.isNull()){
        ERROR1(ERR_NO_INITIALIZED_1, "output georeference");
        return sPREPAREFAILED;
    }
    sz.zsize(_inputRaster->size().zsize());
    _outputRaster->size(sz);
    Envelope env = grf->pixel2Coord(grf->size());
    _outputRaster->envelope(env);
    if ( outputName != sUNDEF)
        _outputRaster->name(outputName);

    if ( _outputRaster->coordinateSystem()->code() == "unknown" || _inputRaster->coordinateSystem()->code() == "unknown"){
        ERROR2(ERR_OPERATION_NOTSUPPORTED2,"resample","coordinatesystem unknown");
        return sPREPAREFAILED;
    }

    QString method = _expression.parm(2).value();
    if ( method.toLower() == "nearestneighbour")
        _method = RasterInterpolator::ipNEARESTNEIGHBOUR;
    else if ( method.toLower() == "bilinear")
        _method = RasterInterpolator::ipBILINEAR;
    else if (  method.toLower() == "bicubic")
        _method =RasterInterpolator::ipBICUBIC;
    else {
        ERROR3(ERR_ILLEGAL_PARM_3,"method",method,"resample");
        return sPREPAREFAILED;
    }
     _outputRaster->stackDefinitionRef() = _inputRaster->stackDefinition();
    for(quint32 i = 0; i < _outputRaster->size().zsize(); ++i){
        QString index = _outputRaster->stackDefinition().index(i);
        _outputRaster->setBandDefinition(index,DataDefinition(_outputRaster->datadef().domain()));
    }

    initialize(_outputRaster->size().linearSize());

    return sPREPARED;
}

quint64 ResampleRaster::createMetadata()
{

    OperationResource operation({"ilwis://operations/resample"});
    operation.setLongName("Resample Raster");
    operation.setSyntax("resample(inputgridcoverage,targetgeoref,interpolation=nearestneighbour|bilinear|!bicubic)");
    operation.setDescription(TR("translates a rastercoverage from one geometry (coordinatesystem+georeference) to another"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itRASTER, TR("input rastercoverage"),TR("input rastercoverage with domain any domain"));
    operation.addInParameter(1,itGEOREF,  TR("target georeference"),TR("the georeference to which the input coverage will be morphed"));
    operation.addInParameter(2,itSTRING, TR("Resampling method"),TR("The method used to aggregate pixels from the input map in the geometry of the output map") );
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage"), TR("output rastercoverage with the domain of the input map"));
    operation.setKeywords("raster, geometry, transformation,georeference,projection,coordinatesystem,reprojection");

    mastercatalog()->addItems({operation});
    return operation.id();
}


