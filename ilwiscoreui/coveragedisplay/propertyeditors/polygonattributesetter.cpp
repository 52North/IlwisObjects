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
#include "raster.h"
#include "featurecoverage.h"
#include "feature.h"
#include "representation.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "polygonattributesetter.h"


using namespace Ilwis;
using namespace Ui;

PolygonPropertySetter::PolygonPropertySetter(VisualAttribute *p) :
    VisualPropertyEditor(p,"polygonpropertysetter",TR("Areas"),QUrl("PolygonProperties.qml"))
{

}

PolygonPropertySetter::PolygonPropertySetter()
{

}

bool PolygonPropertySetter::canUse(const IIlwisObject& obj, const QString& name ) const
{
    if (!obj.isValid())
        return false;
    if(!hasType(obj->ilwisType(), itFEATURE))
        return false;
    Ilwis::IFeatureCoverage features = obj.as<Ilwis::FeatureCoverage>();
    if ( features->featureCount(itPOLYGON) == 0)
        return false;
    return name == VisualAttribute::LAYER_ONLY && !vpmodel()->layer()->isSupportLayer();

}

bool PolygonPropertySetter::showAreas() const
{
    if ( !vpmodel()->layer())
        return false;

    QVariant var = vpmodel()->layer()->vproperty("polygonareas");
    if ( var.isValid())
        return var.toBool();
    return false;

}

void PolygonPropertySetter::setShowAreas(bool yesno)
{
    if ( !vpmodel()->layer())
        return ;

    vpmodel()->layer()->vproperty("polygonareas", yesno);
    vpmodel()->layer()->redraw();

}


VisualPropertyEditor *PolygonPropertySetter::create(VisualAttribute *p)
{
    return new PolygonPropertySetter(p);
}
