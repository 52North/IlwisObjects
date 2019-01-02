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
#include "pixeliterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "opencv.hpp"
#include "opencvhelper.h"
#include "opencvoperation.h"
#include "medianblurfilter.h"

using namespace Ilwis;
using namespace OpenCV;

REGISTER_OPERATION(MedianBlurFilter)

MedianBlurFilter::MedianBlurFilter()
{
}

MedianBlurFilter::MedianBlurFilter(quint64 metaid, const Ilwis::OperationExpression &expr) : OpenCVOperation(metaid,expr  )
{
}

bool MedianBlurFilter::doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const
{
    cv::medianBlur(cvRaster, cvOutputRaster,  _kernelSize);

    return true;
}

Ilwis::OperationImplementation *MedianBlurFilter::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new MedianBlurFilter(metaid,expr);
}

Ilwis::OperationImplementation::State MedianBlurFilter::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    try{
        OperationImplementation::prepare(ctx,st);
        createInputOutputRasters( itCOORDSYSTEM | itGEOREF | itENVELOPE | itDOMAIN);

        OperationHelper::check([&] ()->bool { return hasType(_inputRaster->datadef().domain()->valueType(), itNUMBER | itCONTINUOUSCOLOR | itPALETTECOLOR); },
            {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "wrong data type; Allowed is Source: image or floating-point, 1-channel or color" } );

        _kernelSize = _expression.input<int>(1);

        OperationHelper::check([&] ()->bool { return _kernelSize > 0; },
        {ERR_ILLEGAL_VALUE_2,"kernel size", QString::number(_kernelSize) } );


        return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;
}

quint64 MedianBlurFilter::createMetadata()
{
    OperationResource operation({"ilwis://operations/medianblurfilter"},"opencv");
    operation.setSyntax("medianblurfilter(inputraster, kernelsize)");
    operation.setDescription(TR("smoothes an image using the median filter with the ksize  x ksize aperture"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER , TR("rastercoverage"));
    operation.addInParameter(1,itINTEGER , TR("kernel size"),TR("This is the neighborhood where the local variance will be calculated, and where pixels will contribute (in a weighted manner)"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"),TR("a raster with smoothend features"));
    operation.setKeywords("image processing,raster,noise reduction, filter");

    mastercatalog()->addItems({operation});
    return operation.id();
}
