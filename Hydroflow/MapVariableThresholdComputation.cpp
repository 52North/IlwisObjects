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
#include "blockiterator.h"
#include "pixeliterator.h"
#include "MapVariableThresholdComputation.h"

using namespace Ilwis;
using namespace Hydroflow;


static bool isEven(int elem)
{
    return elem % 2 == 0;
}


VariableThresholdComputationBase::VariableThresholdComputationBase()
{
}

VariableThresholdComputationBase::VariableThresholdComputationBase(quint64 metaid, const Ilwis::OperationExpression& expr) : OperationImplementation(metaid, expr)
{

}


bool VariableThresholdComputationBase::execute(ExecutionContext* ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    /*iterFlag 1. Each element is initialled to zero;
              2. Elements in depres.contributing area are flagged,
                 when the the depres. area are defined;
              3. Elements in flat area flagged to -1*/

    _xsize = _inRaster->size().xsize();
    _ysize = _inRaster->size().ysize();

    quint64 copylist = itRASTERSIZE | itENVELOPE | itINTEGER;
    _iterEmptyRaster = OperationHelperRaster::initialize(_inRaster.as<IlwisObject>(), itRASTER, copylist);


    std::fill(iterOut, iterOut.end(), rUNDEF);

    if (isEven(m_iFilterSize))
        m_iFilterSize += 1;

    bool ok;
    ok = ComputeInternalRelief();


    if (_reliefRaster.isValid())
        ok = executeReliefThresholdComputation();
    else
        ok = executeThresholdComputation();

    if (ok && ctx != 0) 
    {
        if (_reliefRaster.isValid())
        {
            QVariant value;
            value.setValue<IRasterCoverage>(_reliefRaster);
            QString a = _reliefRaster->name();
            ctx->addOutput(symTable, value, _reliefRaster->name(), itRASTER, _reliefRaster->resource());
        }
        else
        {
            QVariant outraster;
            outraster.setValue<IRasterCoverage>(_outRaster);
            ctx->addOutput(symTable, outraster, _outRaster->name(), itRASTER, _outRaster->resource());
        }
    }
    return ok;
}


Ilwis::OperationImplementation* VariableThresholdComputationBase::create(quint64 metaid, const Ilwis::OperationExpression& expr)
{
    return new VariableThresholdComputationBase(metaid, expr);
}


Ilwis::OperationImplementation::State VariableThresholdComputationBase::prepare(ExecutionContext* ctx, const SymbolTable& st)
{
    OperationImplementation::prepare(ctx, st);
    inraster = _expression.parm(0).value();
    QString outputName = _expression.parm(0, false).value();
    m_iFilterSize = _expression.parm(1).value().toInt();
    
    if ((m_iFilterSize < 3))  //must be odd values
        throw ErrorObject(TR("Filter size must be an odd value and greater than 1"));

    if (isEven(m_iFilterSize))
        m_iFilterSize += 1;

    if (!_inRaster.prepare(inraster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2, inraster, "");
        return sPREPAREFAILED;
    }

    int copylist = itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF;
    _outRaster = OperationHelperRaster::initialize(_inRaster.as<IlwisObject>(), itRASTER, copylist);
    if (!_outRaster.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }

    IDomain dom("code=domain:value");
    _outRaster->datadefRef() = DataDefinition(dom);

    for (quint32 i = 0; i < _outRaster->size().zsize(); ++i) {
        QString index = _outRaster->stackDefinition().index(i);
        _outRaster->setBandDefinition(index, DataDefinition(dom));
    }

    return sPREPARED;
 
}


quint64 VariableThresholdComputationBase::createMetadata(OperationResource& operation)
{
    operation.checkAlternateDefinition();
    mastercatalog()->addItems({ operation });
    return operation.id();
}


bool VariableThresholdComputationBase::IsEdgeCell(Pixel pix)
{
    if (pix.y <= 0 || pix.y >= _ysize - 1 ||
        pix.x <= 0 || pix.x >= _xsize - 1)
        return true;
    else
        return false;

}


bool VariableThresholdComputationBase::ComputeInternalRelief()
{
    if (!_inRaster.isValid() || m_iFilterSize == 0)
        return false;

    int iSize = (m_iFilterSize) / 2;
    iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);
    iterDEM = PixelIterator(_inRaster, BoundingBox(), PixelIterator::fXYZ);
    iterOut = PixelIterator(_outRaster, BoundingBox(), PixelIterator::fXYZ);

    PixelIterator inEnd = iterDEM.end();

    while (iterPos != inEnd)
    {
        Pixel pxl = iterPos.position();
        if (*iterDEM(pxl) != rUNDEF)
        {
            std::vector<double> vVal;
            vVal.resize(0);
            for (int i = -iSize; i <= iSize; ++i)
            {
                for (int j = -iSize; j <= iSize; ++j)
                {
                    Pixel pospxl;
                    pospxl.x = pxl.x + j;
                    pospxl.y = pxl.y + i;
                    pospxl.z = 0;

                    if ((IsEdgeCell(pospxl) != true) && (*iterDEM(pospxl) != rUNDEF))
                    {
                        vVal.push_back(*iterDEM(pospxl));
                    }
                }
            }
            if (vVal.size() > 0)
            {
                std::vector<double>::iterator pos = max_element(vVal.begin(), vVal.end());
                double rMax = *pos;
                pos = std::min_element(vVal.begin(), vVal.end());
                double rMin = *pos;
                *iterOut(pxl) = rMax - rMin;
            }
            else
                *iterOut(pxl) = rUNDEF;
        }
        iterPos++;
    }
    return true;
}


bool VariableThresholdComputationBase::ReClassify()
{
    if (!_inRaster.isValid() || m_iFilterSize ==0 )
        return false;

    iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);
    iterDEM = PixelIterator(_inRaster, BoundingBox(), PixelIterator::fXYZ);
    iterOut = PixelIterator(_outRaster, BoundingBox(), PixelIterator::fXYZ);

    PixelIterator inEnd = iterDEM.end();

    while (iterPos != inEnd)
    {
        Pixel pxl = iterPos.position();
        *(iterDEM(pxl)) = rUNDEF;
        if (*iterOut(pxl) != rUNDEF)
        {
            for (int iClass = 0; iClass < m_iClasses; ++iClass)
            {
                if (*iterOut(pxl) <= vThreshold[iClass].UpperRelief)
                {
                    *(iterDEM(pxl)) = vThreshold[iClass].AccValue;
                    break;
                }
            }
        }
        *iterPos++;
    }

    iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);
    int iSize = (m_iFilterSize) / 2;

    while (iterPos != inEnd)
    {
        Pixel pxl = iterPos.position();
        if (*iterDEM(pxl) != rUNDEF)
        {
            std::vector<double> vVal;
            vVal.resize(0);
            for (int i = -iSize; i <= iSize; ++i)
            {
                for (int j = -iSize; j <= iSize; ++j)
                {
                    Pixel pospxl;
                    pospxl.x = pxl.x + j;
                    pospxl.y = pxl.y + i;
                    pospxl.z = 0;

                    if ((IsEdgeCell(pospxl) != true) && (*iterDEM(pospxl) != rUNDEF))
                    {
                        vVal.push_back(*iterDEM(pospxl));
                    }
                }
            }
            int iCout = 0;
            int iMaxCout = 0;
            double rValue;
            for (unsigned int i = 0; i < vVal.size(); ++i)
            {
                iCout = std::count(vVal.begin(), vVal.end(), vVal[i]);
                if (iCout > iMaxCout)
                {
                    iMaxCout = iCout;
                    rValue = vVal[i];
                }
            }
            *iterOut(pxl) = rValue;
        }
        else
            *iterOut(pxl) = rUNDEF;

        *iterPos++;
    }
    return true;
}


bool VariableThresholdComputationBase::executeThresholdComputation()
{
    return ReClassify();
}


bool VariableThresholdComputationBase::executeReliefThresholdComputation()
{
    if (!_reliefRaster.isValid() || m_iFilterSize == 0)
        return false;

    iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);
    iterDEM = PixelIterator(_inRaster, BoundingBox(), PixelIterator::fXYZ);
    iterOut = PixelIterator(_outRaster, BoundingBox(), PixelIterator::fXYZ);
    iterRelief = PixelIterator(_reliefRaster, BoundingBox(), PixelIterator::fXYZ);

    PixelIterator inEnd = iterDEM.end();

    while (iterPos != inEnd)
    {
        Pixel pxl = iterPos.position();
        *iterRelief(pxl) = *iterOut(pxl);
        iterPos++;
    }
   
   return ReClassify();

}


REGISTER_OPERATION(InternalRelief)

InternalRelief::InternalRelief(quint64 metaid, const Ilwis::OperationExpression& expr) : VariableThresholdComputationBase(metaid, expr)
{}

Ilwis::OperationImplementation* InternalRelief::create(quint64 metaid, const Ilwis::OperationExpression& expr)
{
    return new InternalRelief(metaid, expr);
}


Ilwis::OperationImplementation::State InternalRelief::prepare(ExecutionContext* ctx, const SymbolTable& st)
{
    if (VariableThresholdComputationBase::prepare(ctx, st) == sPREPAREFAILED)
        return sPREPAREFAILED;

     QString reliefMapName = inraster + QString("_1");
    _reliefRaster.prepare();
    
    int copylist = itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF;
    _reliefRaster = OperationHelperRaster::initialize(_inRaster.as<IlwisObject>(), itRASTER, copylist);

    if (!_reliefRaster.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }

    IDomain dom("code=domain:value");
    _reliefRaster->datadefRef() = DataDefinition(dom);

    for (quint32 i = 0; i < _reliefRaster->size().zsize(); ++i) {
        QString index = _reliefRaster->stackDefinition().index(i);
        _reliefRaster->setBandDefinition(index, DataDefinition(dom));
    }

    return sPREPARED; 
  
}

quint64 InternalRelief::createMetadata()
{
    OperationResource operation({ "ilwis://operations/InternalRelief" });
    operation.setSyntax("internalrelief(inputraster,filtersize)");
    operation.setDescription(TR("generates a internal relief raster based on the filter size"));

    operation.setInParameterCount({ 2 });
    operation.addInParameter(0, itRASTER, TR("rastercoverage"), TR("input rastercoverage with numeric domain"));
    operation.addInParameter(1, itINT32, TR("filter size"), TR("Type an odd value to define the filter size "), OperationResource::ueCOMBO);

    operation.setOutParameterCount({ 1 });
    operation.addOutParameter(0, itRASTER, TR("output raster"), TR("output raster with the internal relief values"));
    operation.setKeywords("raster,internalrelief");

    return VariableThresholdComputationBase::createMetadata(operation);
}

REGISTER_OPERATION(VariableThresholdComputation)

VariableThresholdComputation::VariableThresholdComputation(quint64 metaid, const Ilwis::OperationExpression& expr) : VariableThresholdComputationBase(metaid, expr)
{}


Ilwis::OperationImplementation* VariableThresholdComputation::create(quint64 metaid, const Ilwis::OperationExpression& expr)
{
    return new VariableThresholdComputation(metaid, expr);
}


Ilwis::OperationImplementation::State VariableThresholdComputation::prepare(ExecutionContext* ctx, const SymbolTable& st)
{
    m_iClasses = _expression.parm(2).value().toInt();
    QString thresholdsstr = _expression.parm(3).value().toLower();
    if ((m_iClasses < 1))  //must be positive values
        throw ErrorObject(TR("Invalid number of classes %li"));

    int ct = 0;
    for (int i = 0; i < m_iClasses; i++)
    {
        ThresholdValues tshval;
        int iup = thresholdsstr.indexOf(",", ct);
        tshval.UpperRelief = thresholdsstr.mid(ct, iup - ct).toDouble();
        ct = iup + 1;

        int idown = thresholdsstr.indexOf(",", ct);
        tshval.AccValue = thresholdsstr.mid(ct, idown - ct).toDouble();
        ct = idown + 1;

        vThreshold.push_back(tshval);
    }
    return VariableThresholdComputationBase::prepare(ctx, st);
}


quint64 VariableThresholdComputation::createMetadata()
{
    OperationResource operation({ "ilwis://operations/VariableThresholdComputation" });
    operation.setSyntax("variablethreshold(inputraster,filtersize,NrOfClasses,UpperBounds_and_ThresholdVals)");
    operation.setDescription(TR("generate a raster containing various stream threshold values that can be used in the Drainage networ exaction operation"));

    operation.setInParameterCount({ 4 });
    operation.addInParameter(0, itRASTER, TR("rastercoverage"), TR("input raster DEM with numeric domain"));
    operation.addInParameter(1, itINT32, TR("filter size"), TR("a value for the size of the window"), OperationResource::ueCOMBO);
    operation.addInParameter(2, itINT32, TR("Nr. of classes"), TR("a value to specify the number of claeeses you wish to obtain in the output threshold raster"), OperationResource::ueCOMBO);
    operation.addInParameter(3, itSTRING, TR("UpperBounds_and_ThresholdVals"), TR("a string to define, for each class, the upper boundary value and the desired output threshold value. Boundary values and threshold values are seperated by commas "), OperationResource::ueCOMBO);

    operation.setOutParameterCount({ 1 });
    operation.addOutParameter(0, itRASTER, TR("output raster"), TR("output raster with variable threshold values"));
    operation.setKeywords("raster,variablethreshold");

    return VariableThresholdComputationBase::createMetadata(operation);

}


