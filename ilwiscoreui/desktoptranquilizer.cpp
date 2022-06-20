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
#include "kernel.h"
#include "ilwisdata.h"
#include "raster.h"
//#include "drawers/attributevisualproperties.h"
#include "tranquilizer.h"
#include "models/uicontextmodel.h"
#include "desktoptranquilizer.h"

using namespace Ilwis;
using namespace Ui;

DesktopTranquilizer::DesktopTranquilizer(const IOOptions& opt, QObject *parent) : BaseTranquilizer(opt,parent)
{

}

DesktopTranquilizer::~DesktopTranquilizer()
{
    if ( _runsInMainThread) //  we do not update in main thread else everything is waiting
        return ;

    emit removeTranquilizer(_id);
}

bool DesktopTranquilizer::update(double step) {
    Locker<std::mutex> lock(_mutex);
    if ( uicontext()->abort() || _end == rUNDEF){
        return false;
    }
    if ( _runsInMainThread) //  we do not update in main thread else everything is waiting
        return true;
    _current += step;
    if ( _current >= _end  || _current < _start){
        emit(updateTranquilizer(_id, _current));
        //std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        _end = rUNDEF;
        emit(removeTranquilizer(_id));
    }else
        emit(updateTranquilizer(_id, _current));

    return true;

}

void DesktopTranquilizer::stop(std::chrono::high_resolution_clock::time_point& start, std::chrono::high_resolution_clock::time_point& end)
{
    if ( _runsInMainThread) //  we do not update in main thread else everything is waiting
        return ;

    Locker<std::mutex> lock(_mutex);
    emit(removeTranquilizer(_id));
}

void DesktopTranquilizer::prepare(const QString &title, const QString &description, double end, double start)
{
    _runsInMainThread = QThread::currentThread() == QCoreApplication::instance()->thread();
    BaseTranquilizer::prepare(title, description,end, start);
    kernel()->connect(this, &BaseTranquilizer::updateTranquilizer, kernel(), &Kernel::changeTranquilizer);
    kernel()->connect(this, &BaseTranquilizer::removeTranquilizer, kernel(), &Kernel::removeTranquilizer);
    kernel()->newTranquilizer(_id, title, description, _start, _end);

}

Tranquilizer *DesktopTranquilizer::create(const IOOptions &opt)
{
    return new DesktopTranquilizer(opt,0);
}



