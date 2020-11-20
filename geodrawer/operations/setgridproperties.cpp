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
#include "setgridproperties.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(SetGridProperties)

SetGridProperties::SetGridProperties()
{

}

SetGridProperties::SetGridProperties(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool SetGridProperties::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    try {
        if (_prepState == sNOTPREPARED)
            if((_prepState = prepare(ctx,symTable)) != sPREPARED)
                return false;

        LayerModel *rootLayer = layerManager()->rootLayer();
        LayerModel *gridlayer =rootLayer->findLayerByName("GridLayer");
        if (!gridlayer)
            return false;

        gridlayer->vproperty("gridlinecolor", qVariantFromValue(_gridColor));
        if ( _distance != -1)
            gridlayer->vproperty("gridcelldistance", qVariantFromValue(_distance));
        if ( _opacity != -1)
           gridlayer->vproperty("gridlineopacity", qVariantFromValue(_opacity));

        return true;

    } catch(const VisualizationError& ){
    }
    return false;
}

Ilwis::OperationImplementation *SetGridProperties::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SetGridProperties(metaid,expr)    ;
}

Ilwis::OperationImplementation::State SetGridProperties::prepare(ExecutionContext *ctx, const SymbolTable &t)
{
    OperationImplementation::prepare(ctx, t);
    if (!getViewId(_expression.input<QString>(0))){
        return sPREPAREFAILED;
    }

    QString linecolor = _expression.input<QString>(1);
    if ( linecolor != sUNDEF){
        QColor clr(linecolor);
        if ( !clr.isValid()){
            kernel()->issues()->log(QString(TR("%1 is not a valid color name/code").arg(linecolor)));
            return sPREPAREFAILED;
        }
        _gridColor = clr;
    }
    double opacity = _expression.input<double>(2);
    if ( opacity != -1){
        if ( opacity <0 || opacity >1){
            kernel()->issues()->log(QString(TR("%1 is not a valid value for opacity").arg(opacity)));
            return sPREPAREFAILED;
        }
        _opacity = opacity;
    }
    double dist = _expression.input<double>(3);
    if ( dist != -1){
        if ( dist <0 ){
            kernel()->issues()->log(QString(TR("%1 is not a valid value for grid distance or number of cells").arg(dist)));
            return sPREPAREFAILED;
        }
        _distance = dist;
    }

    return sPREPARED;
}

quint64 SetGridProperties::createMetadata()
{
    OperationResource operation({"ilwis://operations/setgridproperties"});
    operation.setSyntax("setgridproperties(viewid, linecolor,lineopacity,celldistance[,type])");
    operation.setDescription(TR("sets the visual properties of a grid. The primary or secondary grid. This is indicated by the optional last parameter. By default the primary grid is addressed. The interpretation of celldistance is different for primary and seconday"));
    operation.setInParameterCount({4,5});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1,itSTRING, TR("Grid line color"), TR("Color in which the grid lines are drawn, a question mark if this parameter is not used"));
    operation.addInParameter(2,itDOUBLE , TR("opacity"), TR("The opacity of the grid lines. A -1 if this parameter is not used"));
    operation.addInParameter(3,itDOUBLE , TR("distance/number"), TR("The distance between the gridlines or (for the secondary grid) the number of cells in a primary cell. -1 if this parameter is not used"));
    operation.addOptionalInParameter(4,itSTRING, TR("Grid"), TR("which grid is addressed. primary or secondary. The default is primary"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    operation.checkAlternateDefinition();
    mastercatalog()->addItems({operation});
    return operation.id();
}

