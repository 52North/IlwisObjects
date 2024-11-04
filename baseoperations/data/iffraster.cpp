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

#include <QString>
#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "pixeliterator.h"
#include "ilwisoperation.h"
#include "ifoperation.h"
#include "iffraster.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(IffRaster)

IffRaster::IffRaster()
{
}


IffRaster::IffRaster(quint64 metaid, const Ilwis::OperationExpression &expr) :
    IfOperation(metaid, expr)
{
    _number[0] = _number[1] = rUNDEF;
}

bool IffRaster::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    BoxedAsyncFunc iffunc = [&](const ProcessingBoundingBoxes& box, int threadIdx) -> bool {

        PixelIterator iterOut(_outputGC,threadIdx, box);
        PixelIterator iterIn(_inputGC,threadIdx, box);
        PixelIterator iter1, iter2;
        bool isCoverage1 = _coverages[0].isValid();
        bool isCoverage2 = _coverages[1].isValid();
        if ( isCoverage1)
            iter1 = PixelIterator(_coverages[0].as<RasterCoverage>(), threadIdx, box);
        if ( isCoverage2)
            iter2 = PixelIterator(_coverages[1].as<RasterCoverage>(), threadIdx, box);
        PixelIterator iterEnd = iterOut.end();
        while(iterOut != iterEnd) {
            double v1,v2;
            if ( isCoverage1) {
                v1 = *iter1;
                ++iter1;
            }
            if ( isCoverage2) {
                v2 = *iter2;
                ++iter2;
            }
            if (_number[0] != rUNDEF)
                v1 = _number[0];
            if ( _number[1] != rUNDEF)
                v2 = _number[1];

            *iterOut = *iterIn ? v1 : v2;

            ++iterOut;
            ++iterIn;
        }
        return true;

    };

	bool resource = OperationHelperRaster::execute(ctx, iffunc, { _inputGC, _outputGC });

    if ( resource && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputGC);
		logOperation(_outputGC, _expression, { _inputGC });
        ctx->setOutput(symTable,value,_outputGC->name(),itRASTER,_outputGC->resource());
    }
    return resource;
}

OperationImplementation *IffRaster::create(quint64 metaid, const OperationExpression &expr)
{
    return new IffRaster(metaid, expr);
}

OperationImplementation::State IffRaster::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString raster = _expression.parm(0).value();

    if (!_inputGC.prepare(raster)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
    DataDefinition outputDataDef = findOutputDataDef(_expression);


    OperationHelperRaster helper;
    helper.initialize(_inputGC, _outputGC, itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);

    _outputGC->datadefRef() = outputDataDef;
    for(quint32 i = 0; i < _outputGC->size().zsize(); ++i){
        QString index = _outputGC->stackDefinition().index(i);
        _outputGC->setBandDefinition(index,DataDefinition(outputDataDef.domain()));
    }

    return sPREPARED;
}



quint64 IffRaster::createMetadata()
{
    OperationResource operation({"ilwis://operations/iffraster"});
    operation.setSyntax("iffraster(rastercoverage,outputchoicetrue,outputchoicefalse)");
    operation.setDescription(TR("constructs a new coverage based on a boolean selection described by the boolean map. The true pixels are taken from the first input map, the false pixels from the second map"));
    operation.setInParameterCount({3});
    operation.addInParameter(0, itRASTER,TR("input rastercoverage"), TR("Input map must have boolean domain"));
    operation.addInParameter(1, itNUMBER | itSTRING | itBOOL | itRASTER, TR("first input raster coverage or number"));
    operation.addInParameter(2, itNUMBER | itSTRING | itBOOL | itRASTER, TR("second input raster coverage or number"));
    operation.setOutParameterCount({1});
    operation.addValidation(0,0,"valuetype=bool");
    operation.addOutParameter(0,itRASTER, TR("output raster coverage"),TR("rastercoverage with all pixels that correspond to the true value in the input having a value"));
    operation.setKeywords("raster,condition,boolean");

    operation.checkAlternateDefinition();
    mastercatalog()->addItems({operation});
    return operation.id();
}
