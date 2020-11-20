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
#include "setlayeropacity.h"
#include "uicontextmodel.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(SetLayerOpacity)

SetLayerOpacity::SetLayerOpacity()
{

}

SetLayerOpacity::~SetLayerOpacity()
{

}

SetLayerOpacity::SetLayerOpacity(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool SetLayerOpacity::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;
    _layer->vproperty("opacity", _alpha);
    return true;
}

Ilwis::OperationImplementation *SetLayerOpacity::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SetLayerOpacity(metaid,expr)    ;
}

Ilwis::OperationImplementation::State SetLayerOpacity::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    if (!getViewId(_expression.input<QString>(0))){
        return sPREPAREFAILED;
    }

    _layer =  layerManager()->findLayer(_expression.input<QString>(1).toInt());
    if (!_layer)
        return sPREPAREFAILED;
    float alpha = _expression.parm(2).value().toFloat();
    if (alpha < 0 || alpha > 1)
        return sPREPAREFAILED;
    _alpha = alpha;
    return sPREPARED;
}

quint64 SetLayerOpacity::createMetadata()
{
    OperationResource operation({"ilwis://operations/setlayeropacity"});
    operation.setSyntax("setlayeropacity(viewid, layerindex,opacity)");
    operation.setDescription(TR("Sets the opacity ( alpha channel) of the layer indicated by the index"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1,itSTRING , TR("layer index"));
    operation.addInParameter(2,itDOUBLE , TR("opacity value"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    operation.checkAlternateDefinition();
    mastercatalog()->addItems({operation});
    return operation.id();
}
