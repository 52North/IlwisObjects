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
#include "ilwisdata.h"
#include "geometries.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "coveragedisplay/draweroperation.h"
#include "coveragedisplay/layermodel.h"
#include "coveragedisplay/layermanager.h"
#include "uicontextmodel.h"
#include "setrepresentation.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(SetRepresentation)

SetRepresentation::SetRepresentation()
{

}

SetRepresentation::SetRepresentation(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool SetRepresentation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    try {
        if (_prepState == sNOTPREPARED)
            if((_prepState = prepare(ctx,symTable)) != sPREPARED)
                return false;
        QVariant var1, var2;
        // we must align both domain and representation
        var1.setValue(_representation->domain());
        _layer->vproperty("visualattribute|domain|" + _attributeName,var1);
        var2.setValue(_representation);
        _layer->vproperty("visualattribute|representation|" + _attributeName,var2);
        return true;
    }catch(const ErrorObject& ){
    }
    return false;
}

Ilwis::OperationImplementation *SetRepresentation::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SetRepresentation(metaid, expr);
}

Ilwis::OperationImplementation::State SetRepresentation::prepare(ExecutionContext *ctx, const SymbolTable &t)
{
    OperationImplementation::prepare(ctx, t);
    if (!getViewId(_expression.input<QString>(0))){
        return sPREPAREFAILED;
    }

    _layer =  layerManager()->findLayer(_expression.input<QString>(1).toInt());
    if (!_layer)
        return sPREPAREFAILED;


    // test if this is a existing layer code. every layer has an alpha channel, if it does not exist tha qvariant will be invalid
   QVariant alpha = _layer->vproperty("opacity");
   if ( !alpha.isValid()){
       kernel()->issues()->log(TR("attributefillcolor operation : The drawer can not be found, operation not executed"));
       return sPREPAREFAILED;
   }

    QVariant vrpr =  _layer->vproperty("visualattribute|domain|" + _expression.input<QString>(2));
    if ( !vrpr.isValid()){
        kernel()->issues()->log(QString(TR("Attribute %1 is not a valid attribute for the display layer")).arg(_expression.input<QString>(2)));
        return sPREPAREFAILED;
    }
    IDomain dom = vrpr.value<IDomain>();
    if ( !dom.isValid()){
       kernel()->issues()->log(QString(TR("Attribute %1 is not a valid attribute for the display layer")).arg(_expression.input<QString>(2)));
       return sPREPAREFAILED;
    }
    if ( hasType(dom->ilwisType(), itITEMDOMAIN | itNUMERICDOMAIN)){
       kernel()->issues()->log(QString(TR("Attribute %1 is not a valid attribute for the display layer")).arg(_expression.input<QString>(2)));
       return sPREPAREFAILED;
    }
    _attributeName = _expression.input<QString>(2);
    QString rprTxt = _expression.input<QString>(3);
    if ( rprTxt.indexOf("://") != -1){
        if(!_representation.prepare(rprTxt, itREPRESENTATION)){
            kernel()->issues()->log(QString(TR("%1 is not a valid representation")).arg(rprTxt));
            return sPREPAREFAILED;
        }
    }else {
        QString code = "code=rpr:" + rprTxt;
        if(!_representation.prepare(code)){
            if(!_representation.prepare(rprTxt, itREPRESENTATION)){
                kernel()->issues()->log(QString(TR("%1 is not a valid representation")).arg(rprTxt));
                return sPREPAREFAILED;
            }
        }
    }
    return sPREPARED;
}

quint64 SetRepresentation::createMetadata()
{
    OperationResource operation({"ilwis://operations/setrepresentation"});
    operation.setSyntax("setrepresentation(viewid, drawername-index, attribname,representation)");
    operation.setDescription(TR("sets a representation for the rendering of an attribute of a layer"));
    operation.setInParameterCount({4});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this command has to be applied"));
    operation.addInParameter(1,itSTRING , TR("layer index/code"), TR("location of the layer to be used"));
    operation.addInParameter(2,itSTRING , TR("attribute name"));
    operation.addInParameter(3,itSTRING , TR("representation code or url"), TR("A representation either defined by a code or an url"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization,representation");

    operation.checkAlternateDefinition();
    mastercatalog()->addItems({operation});
    return operation.id();
}

