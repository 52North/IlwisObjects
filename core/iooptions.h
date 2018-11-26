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

#ifndef IOOptions_H
#define IOOptions_H

#include <QVariantMap>
#include "kernel_global.h"

namespace Ilwis {

// optional parameter for the creation process
class KERNELSHARED_EXPORT IOOptions  : public QVariantMap{
public:
    class Option{
        friend class IOOptions;
    public:
        Option(const QString& key, const QVariant& value) : _key(key), _value(value) {}

    private:
        QString _key;
        QVariant _value;

    };

    IOOptions()  {}
    IOOptions(const QString& key, const QVariant& value) { (*this)[key] = value; }
	IOOptions(const QVariantMap& other);

    bool isEmpty() const;

    IOOptions &operator<<(const Option& option);
    IOOptions &addOption(const QPair<QString, QVariant> &item);
    IOOptions &addOption(const QString& key, const QVariant& value);

};



const QString IMPLICITPARMATER="_implicitparm_";
const QString IMPLICITPARMATER0="_implicitparm_0";
const QString IMPLICITPARMATER1="_implicitparm_1";
const QString IMPLICITPARMATER2="_implicitparm_2";
const QString IMPLICITPARMATER3="_implicitparm_3";
}

#endif // IOOptions_H
