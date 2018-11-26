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
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "ilwisobjectconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "downloadmanager.h"
#include "remotecatalogexplorer.h"

using namespace Ilwis;
using namespace Stream;

REGISTER_CATALOGEXPLORER(RemoteCatalogExplorer)

RemoteCatalogExplorer::RemoteCatalogExplorer(const Ilwis::Resource &resource, const IOOptions &options) : CatalogExplorer(resource, options)
{

}

std::vector<Resource> RemoteCatalogExplorer::loadItems(const IOOptions &)
{

    DownloadManager manager(source());
    std::vector<Resource> items = manager.loadItems();

    return items;

}

bool RemoteCatalogExplorer::canUse(const Resource &resource) const
{
    QUrlQuery query(resource.url());
    if ( query.queryItemValue("service") != "ilwisobjects") // can't use anything marked as internal
        return false;
    if ( query.queryItemValue("ilwistype") != "catalog")
        return false;
    return true;
}

QString RemoteCatalogExplorer::provider() const
{
    return "ilwis"    ;
}

Ilwis::CatalogExplorer *RemoteCatalogExplorer::create(const Ilwis::Resource &resource, const Ilwis::IOOptions &options)
{
    return new RemoteCatalogExplorer(resource, options);
}
