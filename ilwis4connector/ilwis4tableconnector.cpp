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
#include "version.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "ilwisobjectconnector.h"
#include "ilwis4connector.h"
#include "ilwis4tableconnector.h"
#include "ilwisobjectconnector.h"
#include "factory.h"
#include "abstractfactory.h"


using namespace Ilwis;
using namespace Ilwis4C;

ConnectorInterface *Ilwis4TableConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions& options)
{
    return new Ilwis4TableConnector(resource, load, options);
}

Ilwis4TableConnector::Ilwis4TableConnector(const Ilwis::Resource &resource, bool load, const IOOptions& options) : Ilwis4Connector(resource, load, options)
{
}

bool Ilwis4TableConnector::store(IlwisObject *obj, const IOOptions &options)
{
    if (!Ilwis4Connector::store(obj, options))
        return false;
    Table *tbl = static_cast<Table *>(obj);

    return true;
}

bool Ilwis4TableConnector::storeData(IlwisObject *obj, const IOOptions &options ){
 
    return true;
}

bool Ilwis4TableConnector::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!Ilwis4Connector::loadMetaData(obj, options))
        return false;
  

    return true;
}

bool Ilwis4TableConnector::loadData(IlwisObject* obj, const IOOptions& options){
    Table *tbl = static_cast<Table *>(obj);
  
    return true;
}
