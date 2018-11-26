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

#include "kernel.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "boundsonlycoordinatesystem.h"

using namespace Ilwis;
BoundsOnlyCoordinateSystem::BoundsOnlyCoordinateSystem()
{
}

BoundsOnlyCoordinateSystem::BoundsOnlyCoordinateSystem(const Ilwis::Resource &resource) :  CoordinateSystem(resource)
{

}

Coordinate BoundsOnlyCoordinateSystem::coord2coord(const ICoordinateSystem &sourceCs, const Coordinate &crdSource) const
{
    if ( sourceCs->id() == id())
        return crdSource;
    return Coordinate();
}

LatLon BoundsOnlyCoordinateSystem::coord2latlon(const Coordinate &crdSource) const
{
    return LatLon();
}

Coordinate BoundsOnlyCoordinateSystem::latlon2coord(const LatLon &ll) const
{
    return Coordinate();
}

bool BoundsOnlyCoordinateSystem::isLatLon() const
{
    return false;
}

bool BoundsOnlyCoordinateSystem::isUnknown() const
{
    return  ( code() == "csy::unknown" || code() == "unknown");
}

IlwisTypes BoundsOnlyCoordinateSystem::ilwisType() const
{
    return itBOUNDSONLYCSY;
}

QString BoundsOnlyCoordinateSystem::toWKT(quint32 spaces) const
{
    return sUNDEF;
}
