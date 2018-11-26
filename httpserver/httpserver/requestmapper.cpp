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

/**
  @file
  @author Stefan Frings
*/
#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"
#include <memory>
#include "httprequesthandler.h"
#include "../requesthandlerfactory.h"
#include "requestmapper.h"

using namespace Ilwis;

RequestMapper::RequestMapper(QObject* parent)
    :HttpRequestHandler(parent) {

}

RequestMapper::~RequestMapper(){
}

void RequestMapper::service(HttpRequest& request, HttpResponse& response) {
    QByteArray path=request.getPath();
    qDebug("RequestMapper: path=%s",path.data());

    UPHTTPRequestHandler handler;
    HTTP::HTTPRequestHandlerFactory *factory = kernel()->factory<HTTP::HTTPRequestHandlerFactory>("ilwis::HTTPRequestHandlerFactory");
    handler.reset(factory->create(path));
    if ( handler)
        handler->service(request, response);
    else{
       handler.reset(factory->create("staticfilecontroller"));
       if ( handler)
           handler->service(request, response);
    }


    qDebug("RequestMapper: finished request");
}
