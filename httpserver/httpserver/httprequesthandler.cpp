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

#include "httprequesthandler.h"

HttpRequestHandler::HttpRequestHandler(QObject* parent)
    : QObject(parent)
{
    qDebug() << "after create"    ;
}

HttpRequestHandler::~HttpRequestHandler() {}

void HttpRequestHandler::service(HttpRequest& request, HttpResponse& response) {
    qCritical("HttpRequestHandler: you need to override the dispatch() function");
    qDebug("HttpRequestHandler: request=%s %s %s",request.getMethod().data(),request.getPath().data(),request.getVersion().data());
    response.setStatus(501,"not implemented");
    response.write("501 not implemented",true);
}

void HttpRequestHandler::error(const QString &text, HttpResponse &response) const
{
    response.setHeader("Content-Type", qPrintable("text/plain"));
    response.write(text.toLocal8Bit());
}
