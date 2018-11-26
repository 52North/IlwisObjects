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

#ifndef MATHHELPER_H
#define MATHHELPER_H

#include "kernel_global.h"

namespace Ilwis {

class Coordinate;

struct KERNELSHARED_EXPORT Coefficient{
    Coefficient(double x=0, double y=0) : _x(x), _y(y){}
    double _x,_y;
};

class KERNELSHARED_EXPORT MathHelper
{
public:
    MathHelper();
    static int findOblique(int iPoints, const std::vector<Coordinate>& independent, const std::vector<Coordinate>& dependent, std::vector<Coefficient>& coef, bool useCols);
    static int findPolynom(int iTerms, int iPoints, const std::vector<Coordinate>& independent, const std::vector<Coordinate>& dependent, std::vector<Coefficient>& coef);
    static NumericRange roundRange(double rmin, double rmax);
    static double round(double r);
    static double roundTo3DecimalDigits(const double value);

    static quint8 lenDecimalDigits(double number);
    static LogicalOperator string2logicalOperator(const QString &op);
    static QString logicalOperator2string(LogicalOperator lo);
};
}
#endif // MATHHELPER_H
