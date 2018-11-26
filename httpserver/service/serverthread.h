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

#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include "qtservice.h"
#include "qtservice_p.h"

class QtServiceBasePrivate;

class ServerThread : public QThread
{
    Q_OBJECT
public:
    explicit ServerThread(QtServiceBasePrivate *service, QObject *parent = 0);


private:
    QtServiceBasePrivate *_service;
    void run() ;


};

class QtServiceStarter1 : public QObject
{
    Q_OBJECT
public:
    QtServiceStarter1(QtServiceBasePrivate *service,QObject *parent = 0)
        : QObject(parent), d_ptr(service) {}
public slots:
    void slotStart()
   {
       d_ptr->startService();
   }
private:
   QtServiceBasePrivate *d_ptr;
};

#endif // SERVERTHREAD_H
