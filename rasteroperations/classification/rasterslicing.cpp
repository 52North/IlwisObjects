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
#include "itemdomain.h"
#include "interval.h"
#include "intervalrange.h"
#include "table.h"
#include "rasterslicing.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(RasterSlicing)

RasterSlicing::RasterSlicing()
{
}


RasterSlicing::RasterSlicing(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{

}

bool RasterSlicing::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    BoxedAsyncFunc sliceFun = [&](const ProcessingBoundingBoxes& box, int threadIdx) -> bool {
        PixelIterator iterOut(_outputRaster, threadIdx, box);
        PixelIterator iterEnd = iterOut.end();
        PixelIterator iterIn(_inputRaster, threadIdx, box);
        qint32 lastIndex = -1;
        double lastMin, lastMax;
        ITable attTable = _outputRaster->attributeTable();
        int colIndex = attTable->columnIndex(_outputRaster->primaryKey());
        Raw lastRaw;
        while(iterOut != iterEnd){
            double v = *iterIn;
            double &outValue = *iterOut;
            if (v == rUNDEF ){
                outValue = rUNDEF;
            } else {
                if ( lastIndex >= 0 && v >= lastMin && v <= lastMax){
                    outValue = lastRaw;
                }else {
                    lastIndex = -1;
                    for(int i=0; i<_bounds.size(); ++i)    {
                        lastMin = _bounds[i]->range().min();
                        lastMax = _bounds[i]->range().max();
                        if ( v >= lastMin && v <= lastMax){
                            lastIndex = i;
                            lastRaw = _bounds[lastIndex]->raw();
                            outValue = lastRaw;
                            attTable->setCell(colIndex,(quint32)outValue,outValue);
                            break;
                        }
                    }
                }
                if ( lastIndex == -1)
                    outValue = rUNDEF;
            }
            ++iterIn;
            ++iterOut;
            updateTranquilizer(iterOut.linearPosition(), 1000);
        }
        return true;

    };

	bool res = OperationHelperRaster::execute(ctx, sliceFun, { _inputRaster, _outputRaster });

    QVariant value;
    value.setValue<IRasterCoverage>(_outputRaster);
	logOperation(_outputRaster, _expression, {_inputRaster, _numericItems});
    ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->resource() );


    return res;

}

OperationImplementation *RasterSlicing::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RasterSlicing(metaid, expr);
}

OperationImplementation::State RasterSlicing::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString raster = _expression.parm(0).value();

    QString outputName = _expression.parm(0,false).value();

    if (!_inputRaster.prepare(raster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
    QString domainName = _expression.parm(1).value();
    if (!_numericItems.prepare(domainName)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,domainName,"");
        return sPREPAREFAILED;
    }
	if (_numericItems->hasOverlaps(false)) {
		kernel()->issues()->log(TR("domain contains overlappeing items which is not allowed"));
		return sPREPAREFAILED;
	}
    _bounds.resize(_numericItems->count());
    int  i =0;
    for(auto& v : _bounds){
        v = _numericItems->item(i++)->as<Interval>();
    }

    IIlwisObject outputObj = OperationHelperRaster::initialize(_inputRaster,itRASTER, itCOORDSYSTEM | itGEOREF);
    if ( !outputObj.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }


    _outputRaster = outputObj.as<RasterCoverage>();
    if ( outputName != sUNDEF)
        _outputRaster->name(outputName);
    std::vector<double> indexes(_outputRaster->size().zsize());
    _outputRaster->setDataDefintions(_numericItems,indexes);
    ITable attributes;

    attributes.prepare();
    attributes->addColumn(_outputRaster->primaryKey(),_numericItems);

    _outputRaster->setAttributes(attributes);

    initialize(_outputRaster->size().linearSize());
    return sPREPARED;
}

quint64 RasterSlicing::createMetadata()
{
    OperationResource operation({"ilwis://operations/sliceraster"});
    operation.setSyntax("sliceraster(inputgridcoverage, IntervalDomain)");
    operation.setDescription(TR("Ranges of values of the input map are grouped together into one output class"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER , TR("input rastercoverage"),TR("input rastercoverage with domain value"));
    operation.addInParameter(1,itITEMDOMAIN , TR("Numeric Item domain"),TR("The domain that used for the classification"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("classified raster"));
    operation.setKeywords("raster, classification");

    operation.checkAlternateDefinition();
    mastercatalog()->addItems({operation});
    return operation.id();
}
