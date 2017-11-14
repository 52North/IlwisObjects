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
#include "setpolygonboundarywidth.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(SetPolygonBoundaryWidth)

SetPolygonBoundaryWidth::SetPolygonBoundaryWidth()
{

}
SetPolygonBoundaryWidth::SetPolygonBoundaryWidth(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{}

bool SetPolygonBoundaryWidth::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    try {
        if (_prepState == sNOTPREPARED)
            if((_prepState = prepare(ctx,symTable)) != sPREPARED)
                return false;
        _layer->vproperty("boundarywidth", QVariant(_boundarywidth));

        return true;
    }catch(const VisualizationError& ){
    }
    return false;
}

Ilwis::OperationImplementation *SetPolygonBoundaryWidth::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SetPolygonBoundaryWidth(metaid, expr)    ;
}

Ilwis::OperationImplementation::State SetPolygonBoundaryWidth::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    if (!getViewId(_expression.input<QString>(0))){
        return sPREPAREFAILED;
    }

    _layer =  layerManager()->findLayer(_expression.input<QString>(1).toInt());
    if (!_layer)
        return sPREPAREFAILED;

    _boundarywidth = _expression.input<double>(2);

    return sPREPARED;
}

quint64 SetPolygonBoundaryWidth::createMetadata()
{
    OperationResource operation({"ilwis://operations/setpolygonboundarywidth"});
    operation.setSyntax("SetPolygonBoundaryWidth(viewid, drawername-index, width[,type])");
    operation.setDescription(TR("sets or resets the width of the boundaries of polygons"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view from which the polygon boundary width should be changed"));
    operation.addInParameter(1,itSTRING|itINTEGER , TR("layer index/code"), TR("location of the layer to be used"));
    operation.addInParameter(2,itDOUBLE , TR("width"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}



