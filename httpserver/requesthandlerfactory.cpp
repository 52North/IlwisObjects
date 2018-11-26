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
#include "factory.h"
#include "abstractfactory.h"
#include <memory>
#include "httpserver/httprequesthandler.h"
#include "requesthandlerfactory.h"

using namespace Ilwis;
using namespace HTTP;

HTTPRequestHandlerFactory::HTTPRequestHandlerFactory() : AbstractFactory("HTTPRequestHandlerFactory","ilwis")
{
}


HttpRequestHandler *HTTPRequestHandlerFactory::create(const QString& type) {
    auto iter = _createMethods.find(type);
    if ( iter != _createMethods.end())
        return iter->second();
    return 0;
}

void HTTPRequestHandlerFactory::addHandler(const QString& type, CreateHandler handler){
    _createMethods[type] = handler;
}

