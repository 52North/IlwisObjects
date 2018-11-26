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

#include <QtPlugin>
#include "kernel.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "symboltable.h"
#include "remotedataaccesshandlermodule.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "catalog.h"
#include "remotedataaccessrequesthandler.h"
#include "remoteoperationrequesthandler.h"
#include "httpserver/requesthandlerfactory.h"




using namespace Ilwis;
using namespace RemoteDataAccess;

RemoteDataAccessHandlerModule::RemoteDataAccessHandlerModule(QObject *parent) :
    Module(parent, "RemoteDataAccessHandlerModule", "iv40","1.0")
{
}

QString RemoteDataAccessHandlerModule::getInterfaceVersion() const
{
    return "iv40";

}

QString RemoteDataAccessHandlerModule::getName() const
{
    return "RemoteDataAccessHandler plugin";
}

QString RemoteDataAccessHandlerModule::getVersion() const
{
    return "1.0";
}

void RemoteDataAccessHandlerModule::finalizePreparation()
{
    HTTP::HTTPRequestHandlerFactory *factory = kernel()->factory<HTTP::HTTPRequestHandlerFactory>("ilwis::HTTPRequestHandlerFactory");
    if ( factory){
        factory->addHandler("/dataaccess", RemoteDataAccessRequestHandler::create);
        factory->addHandler("/operation", RemoteOperationRequestHandler::create);
    }
}

void RemoteDataAccessHandlerModule::prepare()
{

  kernel()->issues()->log("Loaded remote data access module",IssueObject::itMessage);

}





