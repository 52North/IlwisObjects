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

#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "domain.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "containerstatistics.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "internaltableconnector.h"

using namespace Ilwis;
using namespace Internal;

ConnectorInterface *Ilwis::Internal::InternalTableConnector::create(const Ilwis::Resource &resource,bool load,const IOOptions& options)
{
    return new InternalTableConnector(resource, load, options);
}

InternalTableConnector::InternalTableConnector(const Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource, load, options)
{
}

bool InternalTableConnector::loadMetaData(IlwisObject *data, const IOOptions &)
{
    return true;
}


IlwisObject *InternalTableConnector::create() const
{
    return new FlatTable(_resource);
}

bool InternalTableConnector::loadData(IlwisObject *obj, const IOOptions &)
{
    Table *table = static_cast<Table *>(obj);

    if(!table->createTable())
        return false;
    _binaryIsLoaded = true;
    return true;
}

QString InternalTableConnector::provider() const
{
    return "internal";
}
