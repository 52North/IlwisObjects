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
#include "coveragedisplay/layermodel.h"
#include "coveragedisplay/layermanager.h"
#include "uicontextmodel.h"
#include "setpolygonboundarycolor.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(SetPolygonBoundaryColor)

SetPolygonBoundaryColor::SetPolygonBoundaryColor()
{

}

SetPolygonBoundaryColor::~SetPolygonBoundaryColor()
{

}

SetPolygonBoundaryColor::SetPolygonBoundaryColor(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool SetPolygonBoundaryColor::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    try {
        if (_prepState == sNOTPREPARED)
            if((_prepState = prepare(ctx,symTable)) != sPREPARED)
                return false;

         _layer->vproperty("boundarycolor", QVariant(_boundaryColor));

        return true;
    }catch(const VisualizationError& ){
    }
    return false;
}

Ilwis::OperationImplementation *SetPolygonBoundaryColor::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SetPolygonBoundaryColor(metaid, expr)    ;
}

Ilwis::OperationImplementation::State SetPolygonBoundaryColor::prepare(ExecutionContext *ctx, const SymbolTable &t)
{
    OperationImplementation::prepare(ctx, t);
    if (!getViewId(_expression.input<QString>(0))){
        return sPREPAREFAILED;
    }

    _layer =  layerManager()->findLayer(_expression.input<QString>(1).toInt());
    if (!_layer)
        return sPREPAREFAILED;

    _boundaryColor = QColor(_expression.input<QString>(2));
    if (!_boundaryColor.isValid()){
        ERROR2(ERR_ILLEGAL_VALUE_2,TR("color value"), _expression.input<QString>(2));
        return sPREPAREFAILED;
    }

    return sPREPARED;
}

quint64 SetPolygonBoundaryColor::createMetadata()
{
    OperationResource operation({"ilwis://operations/setpolygonboundarycolor"});
    operation.setSyntax("setpolygonboundarycolor(viewid, drawername-index, color-string[,type])");
    operation.setDescription(TR("sets or resets the visibility of a layer"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1,itSTRING , TR("layer index/code"), TR("location of the layer to be used"));
    operation.addInParameter(2,itSTRING , TR("color name"), TR("either an svg color name or a hex based color code (e.g #ff1145)"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}



