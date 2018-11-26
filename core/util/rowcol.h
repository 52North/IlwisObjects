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

#ifndef ROWCOL_H
#define ROWCOL_H

#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT RowCol
{
public:
    RowCol();

    RowCol(double r, double c);
    RowCol(QString& pair);
    double row(bool subpixel=false) const;
    void setRow(double r);
    double col(bool subpixel=false) const;
    void setCol(double c);
    bool operator==(const RowCol& rc) const;
    bool operator!=(const RowCol& rc) const;
    bool isValid() const;
    QString toString() const;

private:
    double _row, _col;


};
}

#define rcUNDEF Ilwis::RowCol(iUNDEF, iUNDEF)

#endif // ROWCOL_H
