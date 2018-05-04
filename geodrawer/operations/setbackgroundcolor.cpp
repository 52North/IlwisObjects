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
#include "setbackgroundcolor.h"


using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(SetBackgroundColor)

SetBackgroundColor::SetBackgroundColor()
{
}

SetBackgroundColor::SetBackgroundColor(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool SetBackgroundColor::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    OperationImplementation::prepare(ctx, symTable);
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    QVariant var= qVariantFromValue(_backgroundColor);
    LayerModel *rootLayer= layerManager()->findLayer(0);
    rootLayer->vproperty("backgroundcolor", var);
    rootLayer->redraw();

    return true;
}

Ilwis::OperationImplementation *SetBackgroundColor::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SetBackgroundColor(metaid, expr);
}

Ilwis::OperationImplementation::State SetBackgroundColor::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    if (!getViewId(_expression.input<QString>(0))){
        return sPREPAREFAILED;
    }

    QString sclr = _expression.input<QString>(1);
    _backgroundColor = QColor(sclr);
    if ( !_backgroundColor.isValid()){
        kernel()->issues()->log(QString(TR("%1 is not a valid hexcode or name for a color").arg(sclr)));
        return sPREPAREFAILED;
    }
    return sPREPARED;
}

quint64 SetBackgroundColor::createMetadata()
{
    OperationResource operation({"ilwis://operations/setbackgroundcolor"});
    operation.setSyntax("setbackgroundcolor(viewid, color)");
    operation.setDescription(TR("sets the background color of a layersview"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view from which the background color must be changed"));
    operation.addInParameter(1,itSTRING , TR("color name"), TR("either an svg color name or a hex based color code (e.g #ff1145)"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}

