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
#include "table.h"
#include "itemdomain.h"
#include "thematicitem.h"
#include "numericdomain.h"
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
    operation.addInParameter(0,itSTRING , TR("Type"),TR("The type of the classifier. Choice: box|mindist|minmahadist|maxlikelihood|spectralangle|priorprob"));
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
        double widenFactor = rUNDEF;
        bool ok;
        if (_expression.parameterCount() > 3) {
            widenFactor = _expression.parm(3).value().toDouble(&ok);
            if (!ok || widenFactor <= 0) {
                ERROR2(ERR_ILLEGAL_VALUE_2, "widen factor", QString(TR("%1; Positive factor needed")).arg(_expression.parm(3).value()));
                return sPREPAREFAILED;
            }
        } else {
            ERROR2(ERR_ILLEGAL_VALUE_2, "widen factor", QString(TR("Missing required parameter \"widen factor\"")));
            return sPREPAREFAILED;
        }
        _classifier.reset(new BoxClassifier(widenFactor, _sampleSet));
    }
    else if (_type == "mindist") {
        double threshold = rUNDEF;
        bool ok;
        if (_expression.parameterCount() > 3) {
            threshold = _expression.parm(3).value().toDouble(&ok);
            if (!ok || threshold <= 0) {
                ERROR2(ERR_ILLEGAL_VALUE_2, "Threshold", QString(TR("%1; Positive threshold needed")).arg(_expression.parm(3).value()));
                return sPREPAREFAILED;
            }
        }
        _classifier.reset(new MinDistClassifier(threshold, _sampleSet));
    }
    else if (_type == "minmahadist") {
        double threshold = rUNDEF;
        bool ok;
        if (_expression.parameterCount() > 3) {
            threshold = _expression.parm(3).value().toDouble(&ok);
            if (!ok || threshold <= 0) {
                ERROR2(ERR_ILLEGAL_VALUE_2, "Threshold", QString(TR("%1; Positive threshold needed")).arg(_expression.parm(3).value()));
                return sPREPAREFAILED;
            }
        }
        _classifier.reset(new MinMahaDistClassifier(threshold, _sampleSet));
    }
    else if (_type == "maxlikelihood") {
        double threshold = rUNDEF;
        bool ok;
        if (_expression.parameterCount() > 3) {
            threshold = _expression.parm(3).value().toDouble(&ok);
            if (!ok || threshold <= 0) {
                ERROR2(ERR_ILLEGAL_VALUE_2, "Threshold", QString(TR("%1; Positive threshold needed")).arg(_expression.parm(3).value()));
                return sPREPAREFAILED;
            }
        }
        _classifier.reset(new MaxLikelihoodClassifier(threshold, _sampleSet));
    }
    else if (_type == "spectralangle") {
        double threshold = rUNDEF;
        bool ok;
        if (_expression.parameterCount() > 3) {
            threshold = _expression.parm(3).value().toDouble(&ok);
            if (!ok || threshold <= 0) {
                ERROR2(ERR_ILLEGAL_VALUE_2, "Threshold", QString(TR("%1; Positive threshold needed")).arg(_expression.parm(3).value()));
                return sPREPAREFAILED;
            }
        }
        _classifier.reset(new SpectralAngleClassifier(threshold, _sampleSet));
    }
    else if (_type == "priorprob") {
        if (_expression.parameterCount() < 5) {
            ERROR2(ERR_ILLEGAL_VALUE_2, "table/column", QString(TR("Missing required parameters; \"table\" and \"column\" are required.")));
            return sPREPAREFAILED;
        }
        QString tableName = _expression.parm(3).value();
        ITable tblPrior;
        if (!tblPrior.prepare(tableName)) {
            ERROR2(ERR_COULD_NOT_LOAD_2, tableName, "");
            return sPREPAREFAILED;
        }
        DataDefinition& tblDef = tblPrior->columndefinitionRef(tblPrior->primaryKey() != sUNDEF ? tblPrior->primaryKey() : COVERAGEKEYCOLUMN).datadef();
        DataDefinition& rasDef = _sampleSet.sampleRaster()->datadefRef();
        if (!tblDef.isCompatibleWith(rasDef)) {
            ERROR2(ERR_ILLEGAL_VALUE_2, "domain", QString(TR("Table with prior probability values must have the same domain as the training raster.")));
            return sPREPAREFAILED;
        }
        QString colPrior = _expression.parm(4).value();
        if (colPrior.size() <= 0) {
            ERROR2(ERR_COULD_NOT_LOAD_2, colPrior, "");
            return sPREPAREFAILED;
        }
        IDomain colDom = tblPrior->columndefinitionRef(colPrior).datadef().domain();
        if (!dynamic_cast<NumericDomain*>(colDom.ptr())) {
            ERROR2(ERR_ILLEGAL_VALUE_2, "domain", colPrior);
            return sPREPAREFAILED;
        }
        double threshold = rUNDEF;
        bool ok;
        if (_expression.parameterCount() > 5) {
            threshold = _expression.parm(5).value().toDouble(&ok);
            if (!ok || threshold <= 0) {
                ERROR2(ERR_ILLEGAL_VALUE_2, "Threshold", QString(TR("%1; Positive threshold needed")).arg(_expression.parm(3).value()));
                return sPREPAREFAILED;
            }
        }
        _classifier.reset(new PriorProbClassifier(threshold, _sampleSet, tblPrior, colPrior));
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
    operation.setInParameterCount({ n, 1 + n, 2 + n, 3 + n});
    operation.addOptionalInParameter(n, itNUMBER | itTABLE, TR("widen-factor|threshold-distance|prior-probability table"),TR("For box: allows you to widen (factor > 1) the boxes that are 'drawn' around class means; for all others: threshold distance when pixels with a spectral signature that is not similar to any of the training classes, should not be classified. For priorprob: the table with the prior probability values"));
    operation.addOptionalInParameter(n + 1, itSTRING, TR("prior-probability column"), TR("For priorprob only. The column in the prior-probability table with the prior probability values. Ideally, the sum of probabilities equals 1. When this is not the case, the program will internally normalize the probability values."));
    operation.addOptionalInParameter(n + 2, itNUMBER, TR("threshold-distance"), TR("For priorprob only. Threshold distance when pixels with a spectral signature that is not similar to any of the training classes, should not be classified."));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage with the domain of the sampleset"));
    operation.setKeywords("classification,raster");

    operation.checkAlternateDefinition();
    mastercatalog()->addItems({operation});
    return operation.id();
}

