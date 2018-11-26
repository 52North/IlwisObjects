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
#include "layertransparencysetter.h"
#include "visualattributemodel.h"


REGISTER_PROPERTYEDITOR("layertransparencysetter",LayerTransparencySetter)

LayerTransparencySetter::LayerTransparencySetter(QObject *parent) :
    VisualAttributeEditor("layertransparencysetter",TR("Transparency"),QUrl("LayerTransparency.qml"), parent)
{

}

LayerTransparencySetter::~LayerTransparencySetter()
{

}

bool LayerTransparencySetter::canUse(const IIlwisObject& obj, const QString& name ) const
{
    if (!obj.isValid())
        return false;
    if(!hasType(obj->ilwisType(), itCOVERAGE))
        return false;
    return name == VisualAttributeModel::LAYER_ONLY;

}

VisualAttributeEditor *LayerTransparencySetter::create()
{
    return new LayerTransparencySetter();
}

float LayerTransparencySetter::transparency() const
{
    if ( layer())
        return 1.0 - layer()->drawer()->attribute("alphachannel").toFloat();
    return 0.0;
}

void LayerTransparencySetter::transparency(float tr)
{
    if (!layer())
        return;

    if ( tr >= 0 && tr <= 1.0){
        layer()->drawer()->setAttribute("alphachannel", 1.0 - tr);
        layer()->drawer()->redraw();
    }
}

