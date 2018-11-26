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
#include "table.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "coveragelayermodel.h"
#include "mapinformationattributesetter.h"

using namespace Ilwis;
using namespace Ui;

MapInformationPropertySetter::MapInformationPropertySetter(VisualAttribute *p) :
    VisualPropertyEditor(p, "mapinfopropertyeditor",TR("Mouse over Info"),QUrl("MapinfoProperties.qml"))
{

}

MapInformationPropertySetter::MapInformationPropertySetter()
{}

bool MapInformationPropertySetter::canUse(const IIlwisObject& obj, const QString& name ) const
{
    if (!obj.isValid())
        return false;
    if(!hasType(obj->ilwisType(), itCOVERAGE))
        return false;
    return name == VisualAttribute::LAYER_ONLY && vpmodel()->layer()->layerType() != itLINELAYER;

}

VisualPropertyEditor *MapInformationPropertySetter::create(VisualAttribute *p)
{
    return new MapInformationPropertySetter(p);
}

bool MapInformationPropertySetter::showInfo() const
{
    if ( vpmodel()->layer())
        return vpmodel()->layer()->as<CoverageLayerModel>()->showInfo();
    return true;
}

void MapInformationPropertySetter::setShowInfo(bool yesno)
{
    if (!vpmodel()->layer())
        return;

    vpmodel()->layer()->as<CoverageLayerModel>()->showInfo(yesno);

}

