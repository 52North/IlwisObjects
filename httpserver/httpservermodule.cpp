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
#include <QSqlQuery>
#include "kernel.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "symboltable.h"
#include "httpservermodule.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "httpserver.h"
#include "httpserver/httprequesthandler.h"
#include "requesthandlerfactory.h"
#include "httpserver/staticfilecontroller.h"


using namespace Ilwis;
using namespace HTTP;

HttpServerModule::HttpServerModule(QObject *parent) :
    Module(parent, "HttpServerModule", "iv40","1.0")
{
}

QString HttpServerModule::getInterfaceVersion() const
{
    return "iv40";

}

void HttpServerModule::prepare()
{
    HTTPRequestHandlerFactory *handlerFactory = new HTTPRequestHandlerFactory();
    kernel()->addFactory(handlerFactory);

    handlerFactory->addHandler("staticfilecontroller",StaticFileController::create);

    kernel()->issues()->log("Loaded module HTTP server",IssueObject::itMessage);


}

QString HttpServerModule::getName() const
{
    return "HTTP Server plugin";
}

QString HttpServerModule::getVersion() const
{
    return "1.0";
}




