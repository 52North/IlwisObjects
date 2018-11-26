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

#include "kernel.h"
#include "errorobject.h"
#include "geometries.h"
#include "astnode.h"
#include "selectornode.h"

using namespace Ilwis;


Selector::Selector(const QString& tp) : ASTNode(tp),
    _variable(sUNDEF),
    _beginCol(sUNDEF),
    _endCol(sUNDEF),
    _beginRec(iUNDEF),
    _endRec(iUNDEF),
    _keyColumns(sUNDEF)
{
    _box = BoundingBox(Pixel(0,0), Pixel(0,0));
}

void Selector::setBounds(const QString& x1, const QString& y1, const QString& x2, const QString& y2){
    _box = BoundingBox(QString("(%1 %2, %3 %4)").arg(x1).arg(y1).arg(x2).arg(y2));
}

void Selector::setSelectorType(const QString& tp){
    _selectorType = tp;
}

BoundingBox Selector::box() const{
    return _box;

}

void Selector::setVariable(const QString& v) {
    _variable = v;
}

QString Selector::variable() const{
    return _variable;
}
QString Selector::selectorType() const{
    return _selectorType;
}

void Selector::beginCol(const QString &col)
{
    _beginCol = col;
}

QString Selector::beginCol() const
{
    return _beginCol;
}

void Selector::endCol(const QString &col)
{
    _endCol = col;
}

QString Selector::endCol() const
{
    return _endCol;
}

void Selector::beginRec(const QString &col)
{
    bool ok;
    _beginRec = col.toInt(&ok);
    if ( !ok)
        throw ErrorObject(TR("invalid record number used"));
}

quint32 Selector::beginRec() const
{
    return _beginRec;
}

void Selector::endRec(const QString &col)
{
    bool ok;
    _endRec = col.toInt(&ok);
    if ( !ok)
        throw ErrorObject(TR("invalid record number used"));
}

quint32 Selector::endRec() const
{
    return _endRec;
}

void Selector::keyColumns(const QString &keys)
{
    _keyColumns = keys;
}

QString Selector::keyColumns() const
{
    return _keyColumns;
}


