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
#include "dilatefilter.h"

using namespace Ilwis;
using namespace OpenCV;

REGISTER_OPERATION(DilateFilter)

DilateFilter::DilateFilter()
{
}

DilateFilter::DilateFilter(quint64 metaid, const Ilwis::OperationExpression &expr) : OpenCVOperation(metaid,expr  )
{
}

bool DilateFilter::doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const
{
    cv::Mat cvKernel = cv::getStructuringElement(_shape,_kernelSize,_anchor);

    cv::dilate(cvRaster, cvOutputRaster, cvKernel, cv::Point(-1,-1),_iterations);

    return true;
}

Ilwis::OperationImplementation *DilateFilter::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new DilateFilter(metaid,expr);
}

Ilwis::OperationImplementation::State DilateFilter::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    try{
        OperationImplementation::prepare(ctx,st);
        createInputOutputRasters( itCOORDSYSTEM | itGEOREF | itENVELOPE | itDOMAIN);

        int iterations = _expression.input<int>(1);
        OperationHelper::check([&] ()->bool { return iterations > 0; },
            {ERR_ILLEGAL_VALUE_2,"iteration number", QString::number(iterations) } );

        _iterations = iterations;

        QString shape = _expression.input<QString>(2).toLower();
        if ( shape == "rectangle")
            _shape = cv::MORPH_RECT;
        else if ( shape == "ellipse")
            _shape = cv::MORPH_ELLIPSE;
        else if ( shape == "cross")
            _shape = cv::MORPH_CROSS;
        else {
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("illegal kernel shape"), shape);
            return sPREPAREFAILED;
        }

        int ksizex = _expression.input<int>(3);
        int ksizey = _expression.input<int>(4);

        OperationHelper::check([&] ()->bool { return ksizex > 0 && ksizey > 0; },
            {ERR_ILLEGAL_VALUE_2,"kernel size", QString::number(ksizex) + "x" + QString::number(ksizey) } );

        _kernelSize = cv::Size(ksizex, ksizey);
        if ( _expression.parameterCount() == 7){
            int x = _expression.input<int>(5);
            int y = _expression.input<int>(6);
            _anchor = cv::Point(x,y);
        }else
            _anchor = cv::Point(-1,-1);

        return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;
}

quint64 DilateFilter::createMetadata()
{
    OperationResource operation({"ilwis://operations/dilatefilter"},"opencv");
    operation.setSyntax("DilateFilter(inputraster,iterations,kerneltype=!rectangle|ellipse|cross,kernel-x-size, kernel-y-size[,x-anchor, y-anchor] ");
    operation.setDescription(TR("Blurs an image using the box filter"));
    operation.setInParameterCount({5,7});
    operation.addInParameter(0,itRASTER , TR("rastercoverage"));
    operation.addInParameter(1,itINTEGER , TR("iterations"),TR("number of times dilation is applied"));
    operation.addInParameter(2,itSTRING , TR("kernel-shape"),TR("structuring element for the filter which can either be a rectangle, ellipse or cross"));
    operation.addInParameter(3,itPOSITIVEINTEGER , TR("kernel width"),TR("width of the structuring element used in the flter"));
    operation.addInParameter(4,itPOSITIVEINTEGER , TR("kernel height"),TR("height of the structuring element used in the flter"));
    operation.addOptionalInParameter(5,itINTEGER , TR("anchors x-position"),TR("anchors position wihtin the kernel. the default is center"));
    operation.addOptionalInParameter(6,itINTEGER , TR("anchors y-position"),TR("anchors position wihtin the kernel. the default is center"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"),TR("a raster with morphology changed of affected structures in the raster"));
    operation.setKeywords("image processing,raster,morphology, filter");

    operation.checkAlternateDefinition();
    mastercatalog()->addItems({operation});
    return operation.id();
}
