#include "kernel.h"
#include "ilwisdata.h"
#include "coverage.h"
#include "symboltable.h"
#include "representation.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "mastercatalog.h"
#include "uicontextmodel.h"
#include "coveragedisplay/draweroperation.h"
#include "coveragedisplay/layermodel.h"
#include "coveragedisplay/layermanager.h"
#include "attributeopacity.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(AttributeOpacity)

AttributeOpacity::AttributeOpacity()
{

}

AttributeOpacity::AttributeOpacity(quint64 metaid, const Ilwis::OperationExpression &expr): DrawerOperation(metaid, expr)
{

}

bool AttributeOpacity::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    QVariant vrpr = _layer->vproperty("visualattribute|representation|" + _attribute );
    IRepresentation rpr = vrpr.value<IRepresentation>();
    if ( hasType(rpr->domain()->ilwisType(), itITEMDOMAIN)){
        IItemDomain dom = rpr->domain().as<ItemDomain<DomainItem>>();
        auto item = dom->item(_item);
        if ( item) {
            Raw raw = item->raw();
            if ( raw != rUNDEF){
                QColor clr = rpr->colors()->value2color(raw);
                clr.setAlphaF(_opacity);
                rpr->colors()->setColor(raw,clr);
            }
            _layer->updateGeometry(true, true);
            _layer->add2ChangedProperties("buffers", true);
        }
    }
    return true;
}

OperationImplementation *AttributeOpacity::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new AttributeOpacity(metaid, expr);
}

OperationImplementation::State AttributeOpacity::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    if (!getViewId(_expression.input<QString>(0))){
        return sPREPAREFAILED;
    }

    _layer =  layerManager()->findLayer(_expression.input<QString>(1).toInt());
    if (!_layer)
        return sPREPAREFAILED;


     // test if this is a existing layer code. every layer has an alpha channel, if it does not exist tha qvariant will be invalid
    QVariant alpha = _layer-> vproperty("opacity");
    if ( !alpha.isValid()){
        kernel()->issues()->log(TR("attributefillcolor operation : The drawer can not be found, operation not executed"));
        return sPREPAREFAILED;
    }
    QString attr = _expression.input<QString>(2);
    // we can only change the colors of attributes with a representation that is not dependent on textdomain or id domain
    QVariant vrpr = _layer->vproperty("visualattribute|representation|" + attr );
    QString item = _expression.input<QString>(3);
    if ( vrpr.isValid()){
        IRepresentation rpr = vrpr.value<IRepresentation>();
        if ( rpr.isValid() && rpr->domain().isValid()){
            if ( hasType(rpr->domain()->ilwisType(), itITEMDOMAIN)){
                IItemDomain dom = rpr->domain().as<ItemDomain<DomainItem>>();
                _attribute = attr;
                if ( dom->contains(item)){
                    _item = item;

                }
            }
        }
    }
    if ( _attribute == sUNDEF){
        kernel()->issues()->log(QString(TR("attributeopacity operation: Not a valid attribute : %1 (wrong domain?)").arg(attr)));
        return sPREPAREFAILED;
    }

    if ( _item == sUNDEF){
        kernel()->issues()->log(QString(TR("attributeopacity operation: Not a valid item : %1 (wrong value?)").arg(attr)));
        return sPREPAREFAILED;
    }
    bool ok;
    _opacity = _expression.input<QString>(4).toDouble(&ok);
    if (!ok || !(_opacity >=0  && _opacity <=1)){
        kernel()->issues()->log(QString(TR("attributeopacity operation : Not a valid opacity value, must be 0 .. 1")).arg(_expression.input<QString>(3)));
        return sPREPAREFAILED;
    }

    return sPREPARED;

}

quint64 AttributeOpacity::createMetadata()
{
    OperationResource operation({"ilwis://operations/attributeopacity"});
    operation.setSyntax("attributeopacity(viewid, layerindex,attribute,itemname, opacity)");
    operation.setDescription(TR("changes the opacity of an representation item of an attribute"));
    operation.setInParameterCount({5});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1,itSTRING, TR("layer code"),TR("The name of the drawer or the type of drawer will be selected based on this parameter"));
    operation.addInParameter(2,itSTRING , TR("attribute name"),TR("The attribute whose representation opacity needs to be changed"));
    operation.addInParameter(3,itSTRING , TR("item name"),TR("the item to be changed"));
    operation.addInParameter(4,itDOUBLE , TR("opacity"),TR("the new color of the representation item"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}
