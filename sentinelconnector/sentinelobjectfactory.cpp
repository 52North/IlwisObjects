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
#include <QStringList>
#include <QSqlError>
#include <QHash>

#include "kernel.h"
#include "ilwiscontext.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "connectorfactory.h"
#include "sentinelobjectfactory.h"

using namespace Ilwis;
using namespace Sentinel;

SentinelObjectFactory::SentinelObjectFactory() : IlwisObjectFactory("IlwisObjectFactory","sentinel","")
{
}


bool SentinelObjectFactory::canUse(const Resource &resource) const 
{
    QUrlQuery query(resource.url());
    if ( query.queryItemValue("service") != "ilwisobjects") {
        if ( resource.url(true).scheme() == "file") { // we might have a streamed object
            QString file = resource.url(true).toLocalFile();
            QFileInfo inf(file);

        }else
            return false;
    }

    IlwisTypes type = resource.ilwisType() ;
	if (type & itRASTER) {
		QString container = resource.container().toString();
		QRegExp re("MTD_MSIL*.xml",Qt::CaseInsensitive, QRegExp::Wildcard);
		int idx = container.indexOf(re);
		return idx != -1;
	}
    //if ( type & itGEOREF)
    //   return true;
    return false;
}

IlwisObject *SentinelObjectFactory::create(const Resource &resource, const IOOptions &options) const
{
    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    IlwisObjectConnector *connector = factory->createFromResource<IlwisObjectConnector>(resource, "sentinel", options);

   if(!connector) {
       kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",resource.name()));
       return 0;
   }
   IlwisObject *object = createObject(connector, options);
   if ( object)
       return object;

   delete connector;
   return 0;
}
