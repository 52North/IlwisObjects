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
#include "coverage.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "columndefinition.h"
#include "table.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "tablemerger.h"
#include "binarymathfeature.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(BinaryMathFeature)

BinaryMathFeature::BinaryMathFeature()
{
}

BinaryMathFeature::BinaryMathFeature(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool BinaryMathFeature::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;


    FeatureIterator iterIn1(_inputFeatureSet1);
    for_each(iterIn1, iterIn1.end(), [&](SPFeatureI feature){
        _outputFeatures->newFeatureFrom(feature, _inputFeatureSet1->coordinateSystem());
    });

    FeatureIterator iterIn2(_inputFeatureSet2);
    for_each(iterIn2, iterIn2.end(), [&](SPFeatureI feature){
        _outputFeatures->newFeatureFrom(feature, _inputFeatureSet2->coordinateSystem());
    });

    ITable attTarget = _outputFeatures->attributeTable();
    _merger.mergeTableData(_inputFeatureSet1->attributeTable(), _inputFeatureSet2->attributeTable(), attTarget);

    QVariant value;
    value.setValue<IFeatureCoverage>(_outputFeatures);
    _outputFeatures->addDescription(_expression.toString());
    ctx->setOutput(symTable,value,_outputFeatures->name(),itFEATURE,_outputFeatures->resource());

    return true;
}

OperationImplementation *BinaryMathFeature::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new BinaryMathFeature(metaid, expr);
}

OperationImplementation::State BinaryMathFeature::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString featureCovName =  _expression.parm(0).value();
    if (!_inputFeatureSet1.prepare(featureCovName)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,featureCovName,"" );
        return sPREPAREFAILED;
    }
    featureCovName =  _expression.parm(1).value();
    if (!_inputFeatureSet2.prepare(featureCovName)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,featureCovName,"" );
        return sPREPAREFAILED;
    }
    bool ok = false;
    if ( ctx->_masterCsy != sUNDEF) {
        ok = _csyTarget.prepare(ctx->_masterCsy);
        if (!ok){
            WARN1(WARN_COULDNT_CREATE_OBJECT_FOR_1,ctx->_masterCsy);
        }
    }
    if (!ok) {
        _csyTarget = _inputFeatureSet1->coordinateSystem();
    }

    Resource resource(_inputFeatureSet1->ilwisType() | _inputFeatureSet2->ilwisType());
    _outputFeatures.prepare(resource);
    Envelope envelope = addEnvelopes();
    _outputFeatures->coordinateSystem(_csyTarget);
    _outputFeatures->envelope(envelope);

    ITable attTable = _merger.mergeMetadataTables(_inputFeatureSet1->attributeTable(), _inputFeatureSet2->attributeTable());
    attTable->recordCount(_inputFeatureSet1->featureCount() + _inputFeatureSet2->featureCount());
    _outputFeatures->setAttributes(attTable);

    QString outname = _expression.parm(0,false).value();
    if ( outname != sUNDEF)
        _outputFeatures->name(outname);

    return sPREPARED;
}

Envelope BinaryMathFeature::addEnvelopes() const {
    Envelope envelope = _csyTarget->convertEnvelope(_inputFeatureSet1->coordinateSystem(),_inputFeatureSet1->envelope());
    envelope += _csyTarget->convertEnvelope(_inputFeatureSet2->coordinateSystem(),_inputFeatureSet2->envelope());
    return envelope;
}

quint64 BinaryMathFeature::createMetadata()
{
    OperationResource operation({"ilwis://operations/binarymathfeatures"});
    operation.setSyntax("binarymathfeatures(featurecoverage1,featurescoverage2,featureoperation=!add|subtract)");
    operation.setDescription(TR("generates a new featurecoverage that puts all the features of both coverages into one coverage"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itFEATURE, TR("first input feature coverage"));
    operation.addInParameter(1,itFEATURE, TR("second input feature coverage"));
    operation.addInParameter(2,itSTRING , TR("Operator"),TR("operator applied to the other 2 input parameters"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itFEATURE, TR("output featurecoverage"));
    operation.setKeywords("features, merge");

    mastercatalog()->addItems({operation});
    return operation.id();
}
