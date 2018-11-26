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
//#include "../../models/attributesetter.h"
#include "raster.h"
#include "featurecoverage.h"
#include "representation.h"
#include "feature.h"
#include "table.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "lineattributesetter.h"


using namespace Ilwis;
using namespace Ui;

LinePropertySetter::LinePropertySetter()
{}

LinePropertySetter::LinePropertySetter(VisualAttribute *p) :
    VisualPropertyEditor(p,"linepropertysetter",TR("Line properties"),QUrl("LineStyleSetter.qml"))
{
}

bool LinePropertySetter::canUse(const IIlwisObject &obj, const QString& name) const
{
    if ( !obj.isValid())
        return false;

    if ( hasType(obj->ilwisType(), itFEATURE)){
        IFeatureCoverage features = obj.as<FeatureCoverage>();
		bool hasPolygons = features->featureCount(itPOLYGON) > 0;
        bool ok = (name == VisualAttribute::LAYER_ONLY) && (features->featureCount(itLINE) || hasPolygons);
		if (hasPolygons && !vpmodel()->layer()->isSupportLayer())
			ok = false;
		return ok;
    }
    return false;
}

VisualPropertyEditor *LinePropertySetter::create(VisualAttribute *p)
{
    return new LinePropertySetter(p);
}

bool LinePropertySetter::showBoundaries() const
{
    if ( !vpmodel()->layer())
        return false;

    QVariant var = vpmodel()->layer()->vproperty("polygonboundaries");
    if ( var.isValid())
        return var.toBool();
    return false;

}

void LinePropertySetter::setShowBoundaries(bool yesno)
{
    if ( !vpmodel()->layer())
        return ;

    vpmodel()->layer()->vproperty("polygonboundaries", yesno);
    vpmodel()->layer()->redraw();

}

void LinePropertySetter::setBoundaryColor(const QColor &clr)
{
    if ( !vpmodel()->layer())
        return ;

    vpmodel()->layer()->vproperty("fixedlinecolor", clr);
}

QColor LinePropertySetter::boundaryColor() const
{
    if ( vpmodel()->layer()){
        QVariant var = vpmodel()->layer()->vproperty("boundarycolor");
        if ( var.isValid())
            return var.value<QColor>();
    }
    return QColor();

}

float LinePropertySetter::boundarywidth() const
{
    if ( !vpmodel()->layer())
        return 1.0;
    QVariant var = vpmodel()->layer()->vproperty("boundarywidth");
    if ( var.isValid())
        return var.toInt();
    return 1.0;

}

void LinePropertySetter::setBoundarywidth(float w)
{
    if ( !vpmodel()->layer())
        return ;

    vpmodel()->layer()->vproperty("boundarywidth", w);
    vpmodel()->layer()->redraw();
}

