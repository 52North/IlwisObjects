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

#include "serverstarter.h"
#include "httplistener.h"
#include "requestmapper.h"
#include "kernel.h"
#include "juliantime.h"
#include <QDir>

/** Name of this application */
#define APPNAME "Ilwis-Objects minimal HTTP server"

/** Publisher of this application */
#define ORGANISATION "52north"

/** Short description of this application */
#define DESCRIPTION "Supporting http server for Ilwis-objects"

/** The HTTP listener of the application */


ServerStarter::~ServerStarter()
{

}


void ServerStarter::start() {
    // Initialize the core application
    _listener.reset(new HttpListener(arguments(), new RequestMapper()));
    Ilwis::Time time = Ilwis::Time::now();
    auto txt =QString("ServiceHelper: Service has started at : %1").arg(time.toString()).toLatin1();
    qWarning(txt);
}

void ServerStarter::stop() {
    // Note that the stop method is not called when you terminate the application abnormally
    // by pressing Ctrl-C or when you kill it by the task manager of your operating system.

    // Deleting the listener here is optionally because QCoreApplication does it already.
    // However, QCoreApplication closes the logger at first, so we would not see the shutdown
    // debug messages, without the following line of code:
    _listener.reset(0);

    qWarning("ServiceHelper: Service has been stopped");
}


ServerStarter::ServerStarter(const std::map<QString, QVariant>& args)
    : QtService(args, APPNAME)
{
    setServiceDescription(DESCRIPTION);
    //setStartupType(AutoStartup);
}




