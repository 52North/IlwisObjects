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
#include "angle.h"
#include "point.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "projectionimplementation.h"
#include "nullprojection.h"

using namespace Ilwis;

NullProjection::NullProjection(const Resource& resource)
{
    setName(resource.name());
    setCode(resource.code());
    setDescription("Null Projection");
}

Coordinate NullProjection::latlon2coord(const LatLon &ll) const
{
    return Coordinate(ll.y(), ll.x(), ll.z());
}

LatLon NullProjection::coord2latlon(const Coordinate &crd) const
{
    return LatLon(crd.y(), crd.x(), crd.z());
}
