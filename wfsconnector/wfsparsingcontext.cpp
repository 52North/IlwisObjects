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

#include <QString>
#include <QVariant>
#include <QMap>

#include "kernel.h"
#include "wfsparsingcontext.h"

using namespace Ilwis;
using namespace Wfs;

WfsParsingContext::WfsParsingContext()
{
}

QString WfsParsingContext::srsName() const
{
    return _srsName;
}

int WfsParsingContext::srsDimension() const
{
    return _srsDimension;
}

QString WfsParsingContext::geometryAtttributeName() const
{
    return _geometryAttributeName;
}

QMap<QString, QString> WfsParsingContext::namespaceMappings() const
{
    return _namespaceMappings;
}

QString WfsParsingContext::featureType() const
{
    return _featureType;
}

Resource WfsParsingContext::resource() const
{
    return _resource;
}

QString WfsParsingContext::currentItem() const
{
    return _currentItem;
}

void WfsParsingContext::setSrsName(QString srsName)
{
    _srsName = srsName;
}

void WfsParsingContext::setSrsDimension(int srsDimension)
{
    _srsDimension = srsDimension;
}

void WfsParsingContext::setGeometryAtttributeName(QString geometryAtttributeName)
{
    _geometryAttributeName = geometryAtttributeName;
}

void WfsParsingContext::addNamespaceMapping(QString prefix, QString uriNamespace)
{
    _namespaceMappings[prefix] = uriNamespace;
}

void WfsParsingContext::setFeatureType(QString featureType)
{
    _featureType = featureType;
}

void WfsParsingContext::setResource(Resource resource)
{
    _resource = resource;
}

void WfsParsingContext::setCurrentItem(QString item)
{
    _currentItem = item;
}



