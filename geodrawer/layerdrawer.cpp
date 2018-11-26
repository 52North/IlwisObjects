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
#include "range.h"
#include "itemrange.h"
#include "colorrange.h"
#include "colorlookup.h"
#include "representation.h"
#include "drawers/attributevisualproperties.h"
#include "table.h"
#include "layerdrawer.h"
#include "drawingcolor.h"
#include <QtGui/QOpenGLContext>
#include "drawers/drawerfactory.h"

using namespace Ilwis;
using namespace Geodrawer;

LayerDrawer::LayerDrawer(const QString& name, DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options) :
    SpatialDataDrawer(name, parentDrawer, rootdrawer, options),
    _activeAttribute(COVERAGEKEYCOLUMN)
{
    valid(true); //TODO: this must move to the prepare once the correct call sequences is in place
}

QString LayerDrawer::activeAttribute() const
{
    return _activeAttribute;
}

void LayerDrawer::setAttribute(const QString &key, const QVariant &attribValue)
{
    SpatialDataDrawer::setAttribute(key, attribValue);
    if ( key == "activevisualattribute"){
        setActiveVisualAttribute(attribValue.toString());
    }
}

QVariant LayerDrawer::attribute(const QString &key) const
{
    QVariant var = SpatialDataDrawer::attribute(key);
    if ( var.isValid())
        return var;

    if ( key == "activevisualattribute"){
        return _activeAttribute;
    }
    return QVariant();
}

QColor LayerDrawer::color(const IRepresentation &rpr, double value, DrawerInterface::ColorValueMeaning cvm)
{
    if ( _activeAttribute != sUNDEF){
        VisualAttribute& attr = _visualProperties[_activeAttribute];
        if ( cvm ==DrawerInterface::cvmFRACTION){
            NumericRange numrange = attr.stretchRange();
            if ( !numrange.isValid()){
                numrange = attr.actualRange();
            }
            value = numrange.min() + numrange.distance() * value;

        }
        QColor clr = attr.value2color(value);
        return clr;
    }
    return QColor();
}

void LayerDrawer::setActiveVisualAttribute(const QString &attr)
{
    if ( !isVisualProperty(attr)){
        return;
    }
    _activeAttribute = attr;
}

void LayerDrawer::cleanUp()
{
}




