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

#include <QUrl>
#include "kernel.h"
#include "errorobject.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"

using namespace Ilwis;

IlwisObjectConnector::IlwisObjectConnector(const Ilwis::Resource &resource, bool, const IOOptions &options) : _resource(resource), _binaryIsLoaded(false), _options(options)
{

}

IlwisObjectConnector::~IlwisObjectConnector()
{

}

IlwisTypes IlwisObjectConnector::type() const
{
    return _resource.ilwisType();
}

Resource &IlwisObjectConnector::sourceRef()
{
    return _resource;
}

const Resource &IlwisObjectConnector::source() const
{
    return _resource;
}

void IlwisObjectConnector::addProperty(const QString &key, const QVariant &value)
{
    if ( key == "connectormode"){
        _mode = (IlwisObject::ConnectorMode)value.toInt();

    }else
        _resource.addProperty(key, value);
}

void IlwisObjectConnector::removeProperty(const QString &key)
{
    _resource.removeProperty(key);
}

bool IlwisObjectConnector::hasProperty(const QString &key) const
{
    return _resource.hasProperty(key);
}

QVariant IlwisObjectConnector::getProperty(const QString &name) const
{
    if ( hasProperty(name))
        return _resource[name];
    return QVariant();
}

const Resource& IlwisObjectConnector::sourceRef() const{
    return _resource;
}

bool IlwisObjectConnector::dataIsLoaded() const
{
    return _binaryIsLoaded;
}

void IlwisObjectConnector::unload()
{

}

IOOptions IlwisObjectConnector::ioOptions() const
{
    return _options;
}

IOOptions &IlwisObjectConnector::ioOptionsRef()
{
    return _options;
}

