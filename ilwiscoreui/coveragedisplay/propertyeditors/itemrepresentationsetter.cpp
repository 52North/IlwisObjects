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
                auto *elem = new RepresentationElementModel(vpmodel()->representation(), item->raw(), item->name(), this);
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
            auto *elem = new RepresentationElementModel(rpr, item->raw(), item->name(), this);
            elem->color(rpr->colors()->value2color(item->raw()));
            _rprElements.push_back(elem);

        }

        emit rprNameChanged();
        emit itemsChanged();
    }
}

void ItemRepresentationSetter::representationChanged() {
	ICoverage cov = coverage();
	if (cov.isValid()) {
		cov->storeAdjustment("representation|" + attributeName(), "dummy");  // the value can be retrieved more efficiently at the implementation of the virtual function
	}
}

