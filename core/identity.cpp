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

#include <QString>
#include <QDataStream>
#include "kernel.h"
#include "identity.h"

using namespace Ilwis;

qint64 Identity::_baseId = 0;


Identity::Identity() : _id(i64UNDEF), _name(sUNDEF), _code(sUNDEF){
}

Identity::Identity(const QString &name, qint64 id, const QString &cde, const QString &descr)
{
    _name = name;
    _id = id;
    _code = cde;
    _description = descr;
}

quint64 Identity::id() const
{
    return _id;
}

void Identity::newId(quint64 base)
{
    _id = base + Identity::_baseId++;
}

QString Identity::name() const
{
    return _name;
}

void Identity::name(const QString &n)
{
    _name =  n;
}

QString Identity::description() const
{
    return _description;
}

void Identity::setDescription(const QString &desc)
{
    _description = desc;
}

bool Identity::store(QDataStream &stream) const
{
    stream << id();
    stream << name();
    stream << code();
    stream << description();

    return true;
}

bool Identity::load(QDataStream &stream)
{
    stream >> _id;
    stream >> _name;
    stream >> _code;
    stream >> _description;

    return true;
}

QString Identity::newAnonymousName()
{
    return QString("%1%2").arg(ANONYMOUS_PREFIX).arg(_baseId++);
}

quint64 Identity::newAnonymousId()
{
    return ++_baseId;
}

void Identity::setBaseId(quint64 base)
{
    _baseId = base;
}

void Identity::code(const QString &code)
{
    _code = code;
}

QString Identity::code() const
{
    return _code;
}

void Identity::prepare(quint64 base) {
    if ( _id == i64UNDEF || base == i64UNDEF) {
        if ( base != i64UNDEF){
            _id = base + Identity::_baseId++;
            name(QString("%1%2").arg(ANONYMOUS_PREFIX).arg(_id)); // default name
        }else{ // second case generates a new id for this object but will not overrule anynames unless they are undefined or anonymous
            _id = Identity::_baseId++;
            if ( _name == sUNDEF || _name.indexOf(ANONYMOUS_PREFIX) != 0)
                name(QString("%1%2").arg(ANONYMOUS_PREFIX).arg(_id)); // default name
        }
    }
}

void Identity::setId(quint64 newid)
{
    _id = newid;
}
