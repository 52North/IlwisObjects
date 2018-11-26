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

#include "coverage.h"
#include "representation.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "layeropacitysetter.h"


using namespace Ilwis;
using namespace Ui;

LayerOpacitySetter::LayerOpacitySetter()
{

}

LayerOpacitySetter::LayerOpacitySetter(VisualAttribute *p) :
    VisualPropertyEditor(p, "layeropacitysetter",TR("Opacity"),QUrl("LayerOpacity.qml"))
{

}

bool LayerOpacitySetter::canUse(const IIlwisObject& obj, const QString& name ) const
{
    if (!obj.isValid())
        return false;
    if(!hasType(obj->ilwisType(), itCOVERAGE))
        return false;
    return name == VisualAttribute::LAYER_ONLY;

}

VisualPropertyEditor *LayerOpacitySetter::create(VisualAttribute *p)
{
    return new LayerOpacitySetter(p);
}

float LayerOpacitySetter::opacity() const
{
	if (vpmodel()->layer())
		return vpmodel()->layer()->opacity();
    return 1.0;
}

void LayerOpacitySetter::opacity(float tr)
{
    if (!vpmodel()->layer())
        return;

    if ( tr >= 0 && tr <= 1.0){
		vpmodel()->layer()->opacity(tr);
    }
}

