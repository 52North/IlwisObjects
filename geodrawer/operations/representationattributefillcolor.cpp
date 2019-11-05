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
#include "representationattributefillcolor.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(RepresentationAttributeFillColor)

RepresentationAttributeFillColor::RepresentationAttributeFillColor()
{

}

RepresentationAttributeFillColor::RepresentationAttributeFillColor(quint64 metaid, const Ilwis::OperationExpression &expr): DrawerOperation(metaid, expr)
{

}

bool RepresentationAttributeFillColor::execute(ExecutionContext *ctx, SymbolTable &symTable)
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
                rpr->colors()->setColor(raw,_color);
            }
			if ( _layer->layerType() == itRASTERLAYER)
				_layer->vproperty("updatetextures", true);
			else
				_layer->add2ChangedProperties("buffers", true);
        }
    }
    return true;
}

OperationImplementation *RepresentationAttributeFillColor::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RepresentationAttributeFillColor(metaid, expr);
}

OperationImplementation::State RepresentationAttributeFillColor::prepare(ExecutionContext *ctx, const SymbolTable &t)
{
    OperationImplementation::prepare(ctx, t);
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
        kernel()->issues()->log(QString(TR("attributefillcolor operation: Not a valid attribute : %1 (wrong domain?)").arg(attr)));
        return sPREPAREFAILED;
    }

    if ( _item == sUNDEF){
        kernel()->issues()->log(QString(TR("attributefillcolor operation: Not a valid item : %1 (wrong value?)").arg(attr)));
        return sPREPAREFAILED;
    }

    _color = QColor(_expression.input<QString>(4));
    if ( !_color.isValid()){
        kernel()->issues()->log(QString(TR("attributefillcolor operation : Not a valid (%1) hexadecimal color code")).arg(_expression.input<QString>(3)));
        return sPREPAREFAILED;
    }

    return sPREPARED;

}

quint64 RepresentationAttributeFillColor::createMetadata()
{
    OperationResource operation({"ilwis://operations/attributefillcolor"});
    operation.setSyntax("attributefillcolor(viewid, layerindex,attribute,itemname, color)");
    operation.setDescription(TR("changes the fill color of an representation item of an attribute"));
    operation.setInParameterCount({5});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1,itSTRING, TR("layer code"),TR("The name of the drawer or the type of drawer will be selected based on this parameter"));
    operation.addInParameter(2,itSTRING , TR("attribute name"),TR("The attribute whose representation color needs to be changed"));
    operation.addInParameter(3,itSTRING , TR("item name"),TR("the item to be changed"));
    operation.addInParameter(4,itSTRING , TR("color"),TR("the new color of the representation item"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}
