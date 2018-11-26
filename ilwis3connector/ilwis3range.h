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

#ifndef ILWIS3RANGE_H
#define ILWIS3RANGE_H

namespace Ilwis {
namespace Ilwis3{

class Ilwis3Range
{

public:
    Ilwis3Range();
    static Range * findRange(const ODF &odf, const QString &section) ;
    static bool minMax2minMax(const Ilwis::ODF &odf, const QString &minmax, double &vmin, double &vmax) ;
    static bool range2MinMax(const ODF &odf,const QString &range, double &vmin, double &vmax, bool &isInteger) ;
    static RawConverter converter(const ODF &odf, const QString &range);
};
}
}

#endif // ILWIS3RANGE_H
