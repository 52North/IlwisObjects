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
#include <QRegularExpression>
#include <functional>
#include <future>
#include <memory>
#include "coverage.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "operationhelper.h"
#include "operationhelperfeatures.h"
#include "featureiterator.h"
#include "geometryhelper.h"
#include "selectionbase.h"
#include "selectionfeatures.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(SelectionFeatures)


//-----------------------------------------------------------------------------------------------

//
SelectionFeatures::SelectionFeatures()
{
}

SelectionFeatures::SelectionFeatures(quint64 metaid, const Ilwis::OperationExpression &expr) : SelectionBase(metaid, expr)
{
}

SelectionFeatures::~SelectionFeatures()
{
}

bool SelectionFeatures::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

   IFeatureCoverage inputFC = _inputObj.as<FeatureCoverage>();

   IFeatureCoverage outputFC = _outputObj.as<FeatureCoverage>();

   std::vector<int> extraAtrrib = organizeAttributes();

   int rec = 0;
   Envelope env;
   for(const auto& feature : inputFC){
       bool ok = true;
       for(const auto& epart : _expressionparts){
           if ( epart._type != ExpressionPart::ptATTRIBUTE){
               bool partOk =  epart.match(feature, this);
               if ( epart._andor != loNONE)
                   ok =  epart._andor == loAND ? ok && partOk : ok || partOk;
               else
                   ok = partOk;
           }
       }
       if ( ok){
           SPFeatureI newFeature = outputFC->newFeatureFrom(feature);
           env += Envelope(*feature->geometry()->getEnvelopeInternal());
           for(int i=0; i < extraAtrrib.size(); ++i){
               _attTable->setCell(extraAtrrib[i], rec, feature->cell(extraAtrrib[i]));
           }
           ++rec;
       }
   }

   outputFC->setAttributes(_attTable);
   outputFC->envelope(env);

   QVariant value;
   value.setValue<IFeatureCoverage>(outputFC);
   logOperation(outputFC, _expression, { inputFC, _inputAttributeTable });
   ctx->setOutput(symTable, value, outputFC->name(), itFEATURE,outputFC->resource());
   return true;
}

Ilwis::OperationImplementation *SelectionFeatures::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SelectionFeatures(metaid, expr);
}

Ilwis::OperationImplementation::State SelectionFeatures::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    IlwisTypes inputType = itFEATURE;
    QString fc = _expression.parm(0).value();
    if (!_inputObj.prepare(fc, inputType)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,fc,"");
        return sPREPAREFAILED;
    }
    IFeatureCoverage inputFC = _inputObj.as<FeatureCoverage>();
    _inputAttributeTable = inputFC->attributeTable();
    quint64 copylist = itCOORDSYSTEM;


    QString selector = _expression.parm(1).value();

    if(!parseSelector(selector, inputFC)){
        kernel()->issues()->log("Invalid selection expression");
        return sPREPAREFAILED;
    }

     _outputObj = OperationHelperFeatures::initialize(_inputObj,inputType, copylist);
     if ( !_outputObj.isValid()) {
         ERROR1(ERR_NO_INITIALIZED_1, "output coverage");
         return sPREPAREFAILED;
     }
     QString outputName = _expression.parm(0,false).value();
     if ( outputName != sUNDEF)
         _outputObj->name(outputName);

     QString url = INTERNAL_CATALOG + "/" + outputName;
     Resource resource(url, itFLATTABLE);
     _attTable.prepare(resource);

     return sPREPARED;
}

quint64 SelectionFeatures::createMetadata()
{
    OperationResource operation({"ilwis://operations/selection"});
    operation.setSyntax("selection(featurecoverage,selection-definition)");
    operation.setLongName("Select features");
    operation.setDescription(TR("the operation select parts of the spatial extent or attributes to create a smaller coverage"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itFEATURE, TR("input feature coverage"),TR("input feature rcoverage with a domain as specified by the selection"));
    operation.addInParameter(1,itSTRING, TR("selection-definition"),TR("Selection can either be attribute, layer index or area definition (e.g. box)"));

    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itFEATURE | itCOLLECTION, TR("Selection"),TR("coverage or index were the selection has been applied") );
    operation.setKeywords("features,selection,submap,attribute,vector");

    operation.checkAlternateDefinition();
    mastercatalog()->addItems({operation});
    return operation.id();

}
