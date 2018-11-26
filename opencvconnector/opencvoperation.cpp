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
#include <functional>
#include "raster.h"
#include "pixeliterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "opencv.hpp"
#include "opencvhelper.h"
#include "opencvoperation.h"

using namespace Ilwis;
using namespace OpenCV;

OpenCVOperation::OpenCVOperation()
{
}

OpenCVOperation::OpenCVOperation(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid,expr  ){

}

bool OpenCVOperation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    try{
        auto indexes = _inputRaster->stackDefinition().indexes();
        _outputRaster->stackDefinitionRef().setSubDefinition(_inputRaster->stackDefinition().domain(), indexes);
        for(auto index : indexes ){
            PixelIterator inputIter = _inputRaster->band(index);

            cv::Mat cvRaster;

            if(!OpenCVHelper::raster2Mat(inputIter,cvRaster))
                return false;

            cv::Mat cvProcessed;
            doOperation(cvRaster, cvProcessed);

            _outputRaster->setBandDefinition(index,_outputRaster->datadef());
            PixelIterator outputIter = _outputRaster->band(index);

            OpenCVHelper::mat2Raster(cvProcessed,outputIter);
        }
        QVariant value;
        value.setValue<IRasterCoverage>(_outputRaster);
        logOperation(_outputRaster,_expression);
        ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->resource() );
        return true;

    }catch(cv::Exception& ex){
        ERROR0(QString::fromStdString(ex.msg));
    }
    return false;
}

void OpenCVOperation::createInputOutputRasters(quint64 copyProperties)
{
    QString outputName = _expression.parm(0,false).value();

    OperationHelper::check([&] ()->bool { return _inputRaster.prepare(_expression.input<QString>(0), itRASTER); },
    {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

    _outputRaster = OperationHelperRaster::initialize(_inputRaster.as<IlwisObject>(), itRASTER, copyProperties);
    OperationHelper::check([&] ()->bool { return _outputRaster.isValid(); },
    {ERR_NO_INITIALIZED_1,"output rastercoverage" } );

    if ( outputName != sUNDEF)
        _outputRaster->name(outputName);

    IDomain dom;
    dom.prepare("code=domain:value");
    _outputRaster->datadefRef().domain(dom);

    for(quint32 i = 0; i < _outputRaster->size().zsize(); ++i){
     QString index = _outputRaster->stackDefinition().index(i);
        _outputRaster->setBandDefinition(index,DataDefinition(dom));
    }
}
