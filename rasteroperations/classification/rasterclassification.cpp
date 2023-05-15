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
#include <unordered_map>
#include "kernel.h"
#include "raster.h"
#include "itemdomain.h"
#include "thematicitem.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "classification/sampleset.h"
#include "eigen3/Eigen/LU"
#include "classifier.h"
#include "rasterclassification.h"

using namespace Ilwis;
using namespace RasterOperations;

RasterClassification::RasterClassification()
{
}

RasterClassification::RasterClassification(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool RasterClassification::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    PixelIterator iterIn(_sampleSet.sampleRasterSet());
    iterIn.setFlow(PixelIterator::fZXY);

    PixelIterator iterOut(_outputRaster);

    bool res = _classifier->classify(iterIn, iterOut);

    if ( res && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputRaster);
        ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->resource(IlwisObject::cmEXTENDED) );
    }
    return res;
}

Ilwis::OperationImplementation::State RasterClassification::prepare(ExecutionContext *, const SymbolTable &sym){
    _type = _expression.parm(0).value();

    QString rasterSetName = _expression.parm(1).value();
    IRasterCoverage rcSet;
    if (!rcSet.prepare(rasterSetName)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,rasterSetName,"");
        return sPREPAREFAILED;
    }

    QString trainingsRaster = _expression.parm(2).value();
    IRasterCoverage rcTraining;
    if (!rcTraining.prepare(trainingsRaster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,trainingsRaster,"");
        return sPREPAREFAILED;
    }

    _sampleSet = SampleSet(rcSet, rcTraining);
    _sampleSet.prepare();

    QString outputName = _expression.parm(0,false).value();

    OperationHelperRaster::initialize(_sampleSet.sampleRasterSet(), _outputRaster, itCOORDSYSTEM | itGEODETICDATUM | itGEOREF | itRASTERSIZE);
    if ( !_outputRaster.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }
    _outputRaster->datadefRef().domain(rcTraining->datadefRef().domain());
    _outputRaster->size(_sampleSet.sampleRasterSet()->size().twod());
    if ( outputName!= sUNDEF)
        _outputRaster->name(outputName);

    return sPREPARED;
}

int RasterClassification::fillOperationMetadata(OperationResource &operation)
{
    operation.addInParameter(0,itSTRING , TR("Type"),TR("The type of the classifier. Choice: box|mindist|minmahadist|maxlikelihood"));
    operation.addInParameter(1,itRASTER , TR("Multiband raster"),TR("Multi band raster to be classified"));
    operation.addInParameter(2,itRASTER , TR("Training set"),TR("Raster containing trainingset(s) of pixels"));
    return 3;
}
//-------------------------------------------------------

REGISTER_OPERATION(RasterClassificationImpl)

RasterClassificationImpl::RasterClassificationImpl(quint64 metaid, const Ilwis::OperationExpression &expr) : RasterClassification(metaid, expr)
{

}

Ilwis::OperationImplementation * RasterClassificationImpl::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RasterClassificationImpl(metaid, expr);
}

Ilwis::OperationImplementation::State RasterClassificationImpl::prepare(ExecutionContext *ctx, const SymbolTable &sym)
{
    OperationImplementation::State prepareState = sNOTPREPARED;

    if ( (prepareState = RasterClassification::prepare(ctx,sym)) != sPREPARED){
        return prepareState;
    }

    if (_type == "box") {
        bool ok;
        double widenFactor = _expression.parm(3).value().toDouble(&ok);
        if (!ok || widenFactor <= 0) {
            ERROR2(ERR_ILLEGAL_VALUE_2, "widen factor", _expression.parm(3).value());
        }
        _classifier.reset(new BoxClassifier(widenFactor, _sampleSet));
    }
    else if (_type == "mindist") {
        bool ok;
        double threshold = _expression.parm(3).value().toDouble(&ok);
        if (!ok || threshold <= 0) {
            ERROR2(ERR_ILLEGAL_VALUE_2, "threshold", _expression.parm(3).value());
        }
        _classifier.reset(new MinDistClassifier(threshold, _sampleSet));
    }
    else if (_type == "minmahadist") {
        bool ok;
        double threshold = _expression.parm(3).value().toDouble(&ok);
        if (!ok || threshold <= 0) {
            ERROR2(ERR_ILLEGAL_VALUE_2, "threshold", _expression.parm(3).value());
        }
        _classifier.reset(new MinMahaDistClassifier(threshold, _sampleSet));
    }
    else if (_type == "maxlikelihood") {
        bool ok;
        double threshold = _expression.parm(3).value().toDouble(&ok);
        if (!ok || threshold <= 0) {
            ERROR2(ERR_ILLEGAL_VALUE_2, "threshold", _expression.parm(3).value());
        }
        _classifier.reset(new MaxLikelihoodClassifier(threshold, _sampleSet));
    }

    if(!_classifier->prepare())
        return sPREPAREFAILED;

    return sPREPARED;
}

quint64 RasterClassificationImpl::createMetadata()
{
    OperationResource operation({"ilwis://operations/classification"});
    operation.setSyntax("classifcation(type,multibandraster,sampleraster,widen-factor|threshold-distance)");
    operation.setDescription(TR("performs a multi-spectral image classification according to training pixels in a sample set"));
    unsigned int n = RasterClassification::fillOperationMetadata(operation);
    operation.setInParameterCount({1 + n});
    operation.addInParameter(n,itNUMBER , TR("widen-factor|threshold-distance"),TR("For box: allows you to widen (factor > 1) the boxes that are 'drawn' around class means; for all others: threshold distance when pixels with a spectral signature that is not similar to any of the training classes, should not be classified."));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage with the domain of the sampleset"));
    operation.setKeywords("classification,raster");

    operation.checkAlternateDefinition();
    mastercatalog()->addItems({operation});
    return operation.id();
}

