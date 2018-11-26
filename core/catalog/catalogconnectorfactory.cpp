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
#include <QStringList>
#include "kernel.h"
#include "factory.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogconnector.h"
#include "catalogconnectorfactory.h"

using namespace Ilwis;

CatalogConnectorFactory::CatalogConnectorFactory() : AbstractFactory("catalogconnectorfactory","ilwis","")
{
}

QList<CatalogConnector *> CatalogConnectorFactory::create(const Resource &location) const{

    QList<CatalogConnector *> finalList;
    QListIterator<ConnectorCreate> iter(_creatorsPerObject);
    while(iter.hasNext()) {
        ConnectorCreate createFunc = iter.next();
        CatalogConnector *conn = static_cast<CatalogConnector *>(createFunc(location,true));
        if ( conn && conn->canUse(location)) {
            finalList.push_back(conn);
        } else
            delete conn;
    }

    return finalList;
}

void CatalogConnectorFactory::add(ConnectorCreate cc)
{
    if (!_creatorsPerObject.contains(cc))
        _creatorsPerObject.push_back(cc);
}

