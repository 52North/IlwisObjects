#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "representation.h"
#include "representationelementmodel.h"
#include "colorlookup.h"
#include "palettecolorlookup.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "itemrepresentationsetter.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_PROPERTYEDITOR("itemrepresentationsetter",ItemRepresentationSetter)

ItemRepresentationSetter::ItemRepresentationSetter()
{

}

ItemRepresentationSetter::ItemRepresentationSetter(VisualAttribute *p) :
    VisualPropertyEditor(p,"itemrepresentationsetter",TR("Legend"),QUrl("ItemRepresentation.qml"))
{

}

void ItemRepresentationSetter::prepare(const IIlwisObject &obj, const DataDefinition &datadef)
{
    VisualPropertyEditor::prepare( obj, datadef);

    if ( vpmodel()->layer()){
        IlwisData<ItemDomain<DomainItem>> itemdom = datadef.domain().as<ItemDomain<DomainItem>>();
        for(auto item : itemdom){
            if (vpmodel()->representation().isValid()){
                QColor clr = vpmodel()->representation()->colors()->value2color(item->raw());
                auto *elem = new RepresentationElementModel(item->raw(), item->name(), this);
                elem->color(vpmodel()->representation()->colors()->value2color(item->raw()));
                _rprElements.push_back(elem);

            }

        }

        emit rprNameChanged();
    }
}

bool ItemRepresentationSetter::canUse(const IIlwisObject &obj, const DataDefinition &def) const{
    if ( def.isValid()){
        if(hasType(def.domain()->ilwisType(), itITEMDOMAIN)){
            if ( hasType(def.domain()->valueType(), itINDEXEDITEM | itNAMEDITEM))
                 return false;
            return true;
        }
    }
    return false;
}

VisualPropertyEditor *ItemRepresentationSetter::create(VisualAttribute *p)
{
    return new ItemRepresentationSetter(p);
}

QQmlListProperty<RepresentationElementModel> ItemRepresentationSetter::representationElements()
{
    return  QQmlListProperty<RepresentationElementModel>(this, _rprElements);
}

void ItemRepresentationSetter::attributesChanged(Raw index, const QVariantMap& values){

    if(values.contains("color")) {
        QColor clr = values["color"].value<QColor>()  ;
        vpmodel()->representation()->colors()->setColor(index,clr);
        VisualPropertyEditor::attributesChanged(index, values);
    }
}

void ItemRepresentationSetter::representationChanged(const IRepresentation& rpr){
    if ( rpr->domain().isValid() && rpr->domain()->ilwisType() == itITEMDOMAIN){
        IlwisData<ItemDomain<DomainItem>> itemdom = rpr->domain().as<ItemDomain<DomainItem>>();
        _rprElements.clear();
        for(auto item : itemdom){
            auto *elem = new RepresentationElementModel(item->raw(), item->name(), this);
            elem->color(rpr->colors()->value2color(item->raw()));
            _rprElements.push_back(elem);

        }

        emit rprNameChanged();
        emit itemsChanged();
    }
}

