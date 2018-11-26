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

#ifndef LOCKER_H
#define LOCKER_H

#include <QDebug>
#include <map>
#include <QThread>
#include "kernel_global.h"

#include <mutex>

typedef std::map<QThread *, int> LockInfo;

KERNELSHARED_EXPORT void printlockmessage(const QString& prefix, const QString& message, const LockInfo& counts) ;
namespace Ilwis {
template<typename MutexType=std::recursive_mutex> class KERNELSHARED_EXPORT Locker : public std::lock_guard<MutexType>
{
public:
    Locker(MutexType &mut) : std::lock_guard<MutexType>(mut){

    }

#ifdef QT_DEBUG
    Locker(MutexType &mut, const QString& debugMessage) : std::lock_guard<MutexType>(mut),_message(debugMessage){
       // _lockcount[QThread::currentThread()]++;
      //  printlockmessage("entering", _message, _lockcount);

    }
    virtual ~Locker() {
//        if (_message != ""){
//            _lockcount[QThread::currentThread()]--;
//            printlockmessage("leaving", _message, _lockcount);
//        }
    }







private:

    QString _message;
   // static LockInfo _lockcount;
#endif
};
//#ifdef QT_DEBUG
//template<class T> LockInfo Locker<T>::_lockcount;
//#endif
}

#endif // LOCKER_H
