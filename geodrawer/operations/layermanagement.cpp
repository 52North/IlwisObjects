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
#include "georeference.h"
#include "coveragedisplay/draweroperation.h"
#include "coveragedisplay/layermanager.h"
#include "layermanagement.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(LayerManagement)

LayerManagement::LayerManagement()
{

}

LayerManagement::~LayerManagement()
{

}

LayerManagement::LayerManagement(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid,expr)
{

}

bool LayerManagement::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    try {
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;
    layerManager()->moveLayer(_layer->parentLayer(), _layer,_action);
    layerManager()->refresh();

    return true;

    } catch(const VisualizationError& ){
    }
    return false;
}

Ilwis::OperationImplementation *LayerManagement::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new  LayerManagement(metaid, expr)   ;
}

Ilwis::OperationImplementation::State LayerManagement::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    if (!getViewId(_expression.input<QString>(0))){
         return sPREPAREFAILED;
    }

    LayerIndex index = _expression.parm(1).value().toInt();
    _layer = layerManager()->findLayer(index);
    if ( _layer){
        kernel()->issues()->log(TR("Illegal layer index:") + _expression.parm(1).value() );
        return sPREPAREFAILED;
    }
    _command = _expression.input<QString>(2).toLower();
    if ( !(_command == "layerup" || _command == "layerdown" || _command == "layerremove")){
        ERROR2(ERR_ILLEGAL_VALUE_2,TR("parameter"), _command);
        return sPREPAREFAILED;
    }
    if ( _command == "layerup"){
        _action = LayerManager::lmUP;
    }
    if ( _command == "layerdown"){
        _action = LayerManager::lmDOWN;
    }
    if ( _command == "layerremove"){
        _action = LayerManager::lmREMOVE;
    }

    return sPREPARED;
}

quint64 LayerManagement::createMetadata()
{
    OperationResource operation({"ilwis://operations/layermanagement"});
    operation.setSyntax("layermanagement(viewid, drawername-index, direction=up | !down|remove)");
    operation.setDescription(TR("changes the position of a layer within the layerview"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1,itSTRING|itINTEGER , TR("layer index"), TR("Index of the layer to be moved or removed"));
    operation.addInParameter(2,itSTRING , TR("action"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}

