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
#include "georeference.h"
#include "coveragedisplay/draweroperation.h"
#include "coveragedisplay/layermodel.h"
#include "coveragedisplay/layermanager.h"
#include "removedrawer.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(RemoveDrawer)

RemoveDrawer::RemoveDrawer()
{
}

RemoveDrawer::RemoveDrawer(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool RemoveDrawer::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

	//TODO : need better implementation
   // layerManager()->moveLayer(_layer->parentLayer(), _layer, LayerModel::lmREMOVE);
	//layerManager()->needRedraw(true);
    return true;
}

Ilwis::OperationImplementation *RemoveDrawer::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RemoveDrawer(metaid, expr);
}

Ilwis::OperationImplementation::State RemoveDrawer::prepare(ExecutionContext *ctx, const SymbolTable &t)
{
    OperationImplementation::prepare(ctx, t);
    if (!getViewId(_expression.input<QString>(0))){
        return sPREPAREFAILED;
    }

    _layer =  layerManager()->findLayer(_expression.input<QString>(1).toInt());
    if (!_layer)
        return sPREPAREFAILED;


    return sPREPARED;
}

quint64 RemoveDrawer::createMetadata()
{
    OperationResource operation({"ilwis://operations/removedrawer"});
    operation.setSyntax("removedrawer(viewid, drawercode)");
    operation.setDescription(TR("changes the view extent"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1,itSTRING , TR("Layer index"), TR("index identifying the drawer"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}



