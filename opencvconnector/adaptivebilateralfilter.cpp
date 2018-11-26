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
#include "adaptivebilateralfilter.h"

using namespace Ilwis;
using namespace OpenCV;

REGISTER_OPERATION(AdaptiveBilateralFilter)

AdaptiveBilateralFilter::AdaptiveBilateralFilter()
{
}

AdaptiveBilateralFilter::AdaptiveBilateralFilter(quint64 metaid, const Ilwis::OperationExpression &expr) : OpenCVOperation(metaid,expr  )
{
}

bool AdaptiveBilateralFilter::doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const
{

    cv::adaptiveBilateralFilter(cvRaster, cvOutputRaster, _kernelSize, _sigmaSpace, _maxSigmaColor);

    return true;
}

Ilwis::OperationImplementation *AdaptiveBilateralFilter::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new AdaptiveBilateralFilter(metaid,expr);
}

Ilwis::OperationImplementation::State AdaptiveBilateralFilter::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    try{
        OperationImplementation::prepare(ctx,st);
        createInputOutputRasters(itCOORDSYSTEM | itGEOREF | itENVELOPE | itDOMAIN);

        OperationHelper::check([&] ()->bool { return hasType(_inputRaster->datadef().domain()->valueType(), itUINT8 | itFLOAT | itDOUBLE | itCONTINUOUSCOLOR | itPALETTECOLOR); },
            {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "wrong data type; Allowed is Source image or floating-point, 1-channel or color" } );


        int ksizex = _expression.input<int>(1);
        int ksizey = _expression.input<int>(2);

        _kernelSize = cv::Size(ksizex, ksizey);
        _sigmaSpace = _expression.input<double>(3);

        _maxSigmaColor = _expression.parameterCount() == 4 ? 20 : _expression.input<int>(4);

        return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;
}

quint64 AdaptiveBilateralFilter::createMetadata()
{
    OperationResource operation({"ilwis://operations/adaptivebilateralfilter"},"opencv");
    operation.setSyntax("adaptivebilateralfilter(inputraster, kernelsizeX, kernelsizeY, sigmaspace[, maxSigmaColor])");
    operation.setDescription(TR("sharpens an image by increasing the slope of the edges without producing overshoot or undershoot"));
    operation.setInParameterCount({4,5});
    operation.addInParameter(0,itRASTER , TR("rastercoverage"));
    operation.addInParameter(1,itINTEGER , TR("kernel width"),TR("This is the neighborhood where the local variance will be calculated, and where pixels will contribute (in a weighted manner)"));
    operation.addInParameter(2,itINTEGER , TR("kernel height"),TR("This is the neighborhood where the local variance will be calculated, and where pixels will contribute (in a weighted manner)"));
    operation.addInParameter(3,itDOUBLE , TR("sigma space"),TR("sigma in the pixel space.Larger value means that farther pixels will influence each other as long as their colors are close enough. When pixel neighborhood>0, it specifies the size regardless of sigmaSpace. Otherwise, d is proportional to sigmaSpace"));
    operation.addOptionalInParameter(4,itDOUBLE , TR("max sigma color"),TR("Maximum allowed sigma color. Larger value of the parameter means that more dissimilar pixels will influence each other, optional"));
    operation.setOutParameterCount({1});
    operation.addValidation(0,0,"domain=numericdomain");
    operation.addOutParameter(0,itRASTER, TR("output raster"),TR("a raster with sharpened features"));
    operation.setKeywords("image processing,raster,sharpen, filter");

    mastercatalog()->addItems({operation});
    return operation.id();
}
