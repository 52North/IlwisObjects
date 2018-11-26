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
#include "kernel.h"
#include "abstractfactory.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "connectorfactory.h"
#include "ilwisobjectfactory.h"

using namespace Ilwis;

IlwisObjectFactory::IlwisObjectFactory(const QString &ty, const QString &sub, const QString &desc) : AbstractFactory(ty,sub,desc)
{
}

std::vector<Resource> IlwisObjectFactory::loadResource(const QUrl &url, IlwisTypes types) const
{
    return std::vector<Resource>();
}

IlwisObject *IlwisObjectFactory::createObject(IlwisObjectConnector* connector, const IOOptions &options) const {
    IlwisObject *object = connector->create();
    if ( object) {
        object->setValid(true);
        if (!object->setConnector(connector, IlwisObject::cmINPUT, options))
            return 0;

        return object;
    }else {
        kernel()->issues()->log(TR("Could not create object"));
    }
    return 0;

}
