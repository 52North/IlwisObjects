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
#include "workflowconnectorobjectfactory.h"

using namespace Ilwis;
using namespace WorkflowConnector;

WorkflowConnectorObjectFactory::WorkflowConnectorObjectFactory() : IlwisObjectFactory("IlwisObjectFactory","WorkflowConnectors","")
{
}


bool WorkflowConnectorObjectFactory::canUse(const Resource &resource) const
{
    if ( resource.url().scheme() == "ilwis") // can't use anything marked as internal
        return false;

    if ( resource.url().scheme() != "file") // can't use anything marked as file
        return false;

    return hasType(resource.ilwisType(), itWORKFLOW);
}

IlwisObject *WorkflowConnectorObjectFactory::create(const Resource &resource, const IOOptions &options) const
{
    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    QFileInfo inf(resource.toLocalFile());
    QFileInfo container(inf.absolutePath());
    IlwisObjectConnector *connector = 0;

   // if ( WorkflowTableConnector::knownSuffix(inf.suffix()) || WorkflowTableConnector::knownSuffix(container.suffix()))
        connector = factory->createFromResource<IlwisObjectConnector>(resource, "workflow", options);

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



