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

#ifndef NULLPROJECTION_H
#define NULLPROJECTION_H

namespace Ilwis {
class KERNELSHARED_EXPORT NullProjection : public Projection
{
public:
    NullProjection(const Ilwis::Resource &resource);
    Coordinate latlon2coord(const LatLon &ll) const;
    LatLon coord2latlon(const Coordinate &crd) const;
};
}

#endif // NULLPROJECTION_H
