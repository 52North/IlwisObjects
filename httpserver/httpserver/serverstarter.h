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

#ifndef STARTUP_H
#define STARTUP_H

#include <memory>
#include <QtCore/QCoreApplication>
#include <QStringList>
#include "httpserver/service/qtservice.h"

class HttpListener;

/**
  Helper class to install and run the application as a windows
  service.
*/
class ServerStarter : public QtService
{
public:

    /** Constructor */
    ServerStarter(const std::map<QString, QVariant> &args = std::map<QString, QVariant>());
    ~ServerStarter();

protected:

    /** Start the service */
    void start();

    /**
      Stop the service gracefully.
      @warning This method is not called when the program terminates
      abnormally, e.g. after a fatal error, or when killed from outside.
    */
    void stop();

private:
    std::unique_ptr<HttpListener>  _listener;

};

#endif // STARTUP_H
