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
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorfactory.h"
#include "catalogconnectorfactory.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "workflowconnectorobjectfactory.h"
#include "connectorinterface.h"
#include "ilwiscontext.h"
//#include "ilwisdata.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "operationExpression.h"
#include "operation.h"
#include "jsonconfig.h"
#include "modeller/workflownode.h"
#include "modeller/workflow.h"
#include "workflowconnectormodule.h"
#include "workflowjsonconnector.h"

using namespace Ilwis;
using namespace WorkflowConnector;

WorkflowConnectorModule::WorkflowConnectorModule(QObject *parent) : Module(parent, "WorkflowConnectorModule", "iv40","1.0")
{

}

QString WorkflowConnectorModule::getInterfaceVersion() const
{
      return "iv40";
}

QString WorkflowConnectorModule::getName() const
{
    return "JSonWorkflow plugin";
}

QString WorkflowConnectorModule::getVersion() const
{
    return "1.0";
}

void WorkflowConnectorModule::finalizePreparation()
{

}

void WorkflowConnectorModule::prepare()
{
    WorkflowConnectorObjectFactory *factory = new WorkflowConnectorObjectFactory();
    factory->prepare();
    kernel()->addFactory(factory );

    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return;

    cfactory->addCreator(itWORKFLOW, "Json", WorkflowJSONConnector::create);
    cfactory->addCreator("workflow", "Json", WorkflowJSONConnector::create);

    kernel()->issues()->log("Loaded Workflow module",IssueObject::itMessage);

}
