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

#include "layerinfoitem.h"
using namespace Ilwis;
using namespace Ui;

LayerInfoItem::LayerInfoItem()
{

}

LayerInfoItem::LayerInfoItem(const QString& name, const QString &val, QObject *parent) :
    QObject(parent),
    _name(name),
    _value(val)
{

}

LayerInfoItem::~LayerInfoItem()
{

}

QString LayerInfoItem::name() const
{
    return _name;
}

QString LayerInfoItem::value() const
{
    return _value;
}

QString LayerInfoItem::icon() const
{
    return _icon;
}

void LayerInfoItem::icon(const QString &ic)
{
    _icon = ic;
}

bool LayerInfoItem::layerHeader() const
{
    return _layerHeader;
}

void LayerInfoItem::layerHeader(bool yesno)
{
    _layerHeader = yesno;
}

