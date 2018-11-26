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

#include <QColor>
#include <QString>
#include <QSharedPointer>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "itemrange.h"
#include "domainitem.h"
#include "datadefinition.h"
#include "colorrange.h"
#include "coloritem.h"

using namespace Ilwis;

ColorItem::ColorItem(const QColor& clr) : _color(clr)
{
}

IlwisTypes ColorItem::valueType() const
{
    return  ColorItem::valueTypeS();
}

QColor ColorItem::color() const
{
    return _color;
}

void ColorItem::color(const QColor clr)
{
    _color = clr;
}

IlwisTypes ColorItem::valueTypeS()
{
    return itPALETTECOLOR;
}

ItemRange *ColorItem::createRange()
{
    return new ColorPalette();
}

DomainItem *ColorItem::clone() const
{
    return new ColorItem(_color);
}

QString ColorItem::name() const
{
    if(!isValid())
        return sUNDEF;
    return _color.name();
}

bool Ilwis::ColorItem::isValid() const
{
    return _color != clrUNDEF;
}



