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
#include "coverage.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "mastercatalog.h"
#include "uicontextmodel.h"
#include "coveragedisplay/draweroperation.h"
#include "cleardrawers.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(ClearDrawers)

ClearDrawers::ClearDrawers()
{
}

ClearDrawers::ClearDrawers(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool ClearDrawers::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    LayerManager *manager = layerManager();
    manager->clearLayers(_parenttLayer);
    return true;
}

Ilwis::OperationImplementation *ClearDrawers::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ClearDrawers(metaid, expr);
}

Ilwis::OperationImplementation::State ClearDrawers::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    if (!getViewId(_expression.input<QString>(0))){
        return sPREPAREFAILED;
    }
    QString parent = _expression.input<QString>(1);
    if ( parent != ""){
        LayerIndex idx = parent.toInt();
        if ( !(_parentLayer = layerManager()->findLayer(idx))){
            kernel()->issues()->log(TR("The parent of the to be cleared layer doesnt exist:") + parent);
            return sPREPAREFAILED;
        }
    }

    return sPREPARED;
}

quint64 ClearDrawers::createMetadata()
{
    OperationResource operation({"ilwis://operations/cleardrawers"});
    operation.setSyntax("clearDrawers(viewerid[, layer])");
    operation.setDescription(TR("removes all the drawers and resets the coordinatesystem(if needed)"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(0,itSTRING , TR("Layer"),TR("layer from which all child layers will be removed. It is not present all layers from the global layer will be removed"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}





