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

#include <QThread>
#include <QCoreApplication>
#include <iostream>
#include "kernel.h"
#include "ilwiscontext.h"
#include "errorobject.h"
#include "factory.h"
#include "abstractfactory.h"
#include "tranquilizer.h"
#include "consoletranquilizer.h"

using namespace Ilwis;

ConsoleTranquilizer::~ConsoleTranquilizer()
{
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    stop(end,end);
}

// This tranquilizer will generate a progress indicator in a terminal window
// When fully processed it will write:
// 0..10..20..30..40..50..60..70..80..90..100
// The output is streamed to the stdout channel
// Note that when the total is less then 30, the indicator will not reach the 100 mark
ConsoleTranquilizer::ConsoleTranquilizer(const IOOptions &opt, QObject *parent) : BaseTranquilizer(opt, parent)
{
    _inc = 0.0;
    _next = 0.0;
    _count = 0;
}

void ConsoleTranquilizer::prepare(const QString &title, const QString &description, double end, double start)
{
    BaseTranquilizer::prepare(title, description, end, start);

    // handle case where existing tranquilizer is re-used
    if (_count > 0)
        std::cout << std::endl;

     _inc = (end - start) / 30;
    _next = 0.0;
    _count = 0;

    if (! hasType(context()->runMode(), Ilwis::rmNOUI)) {
        if ( _title != "")
            std::cout << title.toStdString() << ": ";
    }
}

bool ConsoleTranquilizer::update(double howfar)
{
    if (howfar < 1) {
        //std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        //stop(end);
        return false;
    }

    _current += howfar;

    if (! hasType(context()->runMode(), Ilwis::rmNOUI)) {
        if (_current >= _next) {
             if (_count % 3 == 0)
                 std::cout << _count * 10 / 3;   // round to integer between 0 and 100 in steps of 10
             else
                 std::cout << ".";
             _count++;
             _next += _inc;
         }
    }

    return true;
}

bool ConsoleTranquilizer::inform(QString msg)
{
    if ( _title != "")
        std::cout << msg.toStdString();
    return true;
}

void ConsoleTranquilizer::stop(std::chrono::high_resolution_clock::time_point& start, std::chrono::high_resolution_clock::time_point &end)
{
    _inc = 0.0;
    _next = 0.0;
    _count = 0;
    _start = 0.0;
    _end = 0.0;
    if (! hasType(context()->runMode(), Ilwis::rmNOUI)) {
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        if ( time_span.count() > 0){
            QString res = " in " + QString::number(time_span.count()) + " seconds";
            std::cout << res.toStdString();
        }
        std::cerr << std::endl;
    }
}

Tranquilizer *ConsoleTranquilizer::create(const IOOptions &opt)
{
    return new ConsoleTranquilizer(opt, 0);
}




