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

#include <QSqlQuery>
#include <QSqlError>
#include <QUrl>
#include <QSqlRecord>
#include "kernel.h"
#include "identity.h"
#include "resource.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "internalcatalogconnector.h"

using namespace Ilwis;
using namespace Internal;

ConnectorInterface *InternalCatalogConnector::create(const Resource& resource, bool load ) {
    if ( resource.url().scheme() == "ilwis")
        return new InternalCatalogConnector(resource, load);
    return nullptr;

}

//InternalCatalogConnector::InternalCatalogConnector(const Resource& resource, bool load ) :CatalogConnector(Resource(QUrl("ilwis://internalcatalog"),itCATALOG), load)
InternalCatalogConnector::InternalCatalogConnector(const Resource& resource, bool load ) :CatalogConnector(resource, load)
{
}

InternalCatalogConnector::~InternalCatalogConnector()
{

}

std::vector<QUrl> InternalCatalogConnector::sources(const QStringList &, int ) const
{
    //TODO: full list??
    return std::vector<QUrl>();
}



bool InternalCatalogConnector::canUse(const Resource &res) const
{
    return res.url().scheme() == "ilwis";
}

QString InternalCatalogConnector::provider() const
{
    return "internal";
}

QFileInfo InternalCatalogConnector::toLocalFile(const QUrl& url) const {
    QFileInfo inf = url.toLocalFile();
    return inf;
}

bool InternalCatalogConnector::loadItems()
{
    return true;
}

bool InternalCatalogConnector::isValid() const{
    return true;
}



