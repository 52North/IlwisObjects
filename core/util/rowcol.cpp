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
#include <QRegExp>
#include <QStringList>
#include "ilwis.h"
#include "rowcol.h"

using namespace Ilwis;

RowCol::RowCol() : _row(rUNDEF), _col(rUNDEF)
{
}

RowCol::RowCol(double r, double c)
{
  _row = r;
  _col = c;
}

RowCol::RowCol(QString &pair)
{
    QStringList parts = pair.split(QRegExp("[\\s,;]"));
    if ( parts.size() == 2) {
        _row = parts[0].toDouble();
        _col = parts[1].toDouble();
    } else {
        *this = rcUNDEF;
    }
}

double RowCol::row(bool subpixel) const
{
    if ( subpixel)
        return _row;
    return (qint32)_row;
}

void RowCol::setRow(double r)
{
    _row = r;
}

double RowCol::col(bool subpixel) const
{
    if( subpixel)
        return _col;
    return (qint32)_col;
}

void RowCol::setCol(double c)
{
    _col = c;
}

bool RowCol::operator ==(const RowCol &rc) const
{
    if ( isValid())
        return rc.row() == row() && rc.col() == col();
    return false;
}

bool RowCol::operator !=(const RowCol &rc) const
{
    return ! (operator ==(rc));
}

bool RowCol::isValid() const
{
    return _row != rUNDEF && _col != rUNDEF;
}

QString RowCol::toString() const
{
    return QString("(%1,%2)").arg(_row,_col);
}
