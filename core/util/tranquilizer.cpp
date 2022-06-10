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
#include "errorobject.h"
#include "factory.h"
#include "abstractfactory.h"
#include "tranquilizerfactory.h"
#include "tranquilizer.h"

using namespace Ilwis;

quint64 BaseTranquilizer::_trqId = 0;

BaseTranquilizer::BaseTranquilizer(const IOOptions &,QObject *parent) : Tranquilizer(parent),
 _id(i64UNDEF),_start(rUNDEF),_end(rUNDEF), _current(rUNDEF)
{

}

BaseTranquilizer::~BaseTranquilizer()
{
}

void BaseTranquilizer::prepare(const QString &title, const QString &description, double end, double start)
{
    _title = title;
    _desc = description;
    _end = end;
    _start = start;
    _current = _start;
    if ( _id == i64UNDEF)
        _id = _trqId++;
}

void BaseTranquilizer::stopTranquilizer()
{
    if ( QThread::currentThread() != QCoreApplication::instance()->thread()){

    }
}

void BaseTranquilizer::end(double number)
{
    _end = number;
}

double BaseTranquilizer::end() const
{
    return _end;
}

double BaseTranquilizer::current() const
{
    return _current;
}

void BaseTranquilizer::current(double cur)
{
    _current = cur;
}

//-------------------------------------------------------------
TranquilizerWorker::TranquilizerWorker()
{

}

void TranquilizerWorker::updateTranquilizer(quint64 id, double amount)
{
    emit sendUpdateTranquilizer(id, amount);
}

void TranquilizerWorker::createTranquilizer(quint64 id, const QString &title, const QString &description, double start, double end)
{
    emit sendCreateTranquilizer(id, title, description, start, end);
}

void TranquilizerWorker::removeTranquilizer(quint64 id)
{
    emit sendRemoveTranquilizer(id);
}

//-------------------------------------
Tranquilizer::Tranquilizer(QObject *parent) : QObject(parent)
{

}

Tranquilizer::~Tranquilizer()
{

}

Tranquilizer *Tranquilizer::create(int mode, const IOOptions& options)
{
    TranquilizerFactory *factory = kernel()->factory<TranquilizerFactory>("ilwis::tranquilizerfactory");
    if (!factory){
        return 0;
    }
    return factory->create<>(mode, options);
}

//------------------------------------------------------
EmptyTranquilizer::EmptyTranquilizer(const IOOptions &opt, QObject *parent) : BaseTranquilizer(opt, parent)
{

}

void EmptyTranquilizer::prepare(const QString &title, const QString &description, double end, double start)
{
    BaseTranquilizer::prepare(title, description, end, start);
}

bool EmptyTranquilizer::update(double )
{
    return true;
}

void EmptyTranquilizer::stop(std::chrono::high_resolution_clock::time_point& start, std::chrono::high_resolution_clock::time_point& end)
{

}

Tranquilizer *EmptyTranquilizer::create(const IOOptions &opt)
{
    return new EmptyTranquilizer(opt,0);
}
