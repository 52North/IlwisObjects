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

#include <QStringList>
#include "kernel.h"
#include "geometries.h"
#include <type_traits>
#include "polygon.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "geometry.h"
#include "geometryoperations.h"

using namespace Ilwis;

Within::Within()
{
}

GeometryMethod *Within::create() const{
        return new Within();
}

bool Within::execute(bool& outBool, const Geometry& container, const Geometry& containee) const{
    //until now only for point.within(polygon)
    outBool = false;
    if ( container.geometryType() == itPOLYGON && containee.geometryType() == itPOINT) {
        const Polygon& pol = container.toType<Polygon>();
        Point2D<double> p;
        const GeometryType& point = containee.geometry();
        if (point.which() == 0)
            p = containee.toType<Pixel>();
        else if (point.which() == 1)
            p = containee.toType<Coordinate2d >();
        else if (point.which() == 2)
            p = containee.toType<Coordinate>();
        outBool =  boost::geometry::within(p, pol);
    }
    return false;
}

//--------------------------------------------------------
Area::Area()
{
}

GeometryMethod *Area::create() const{
        return new Area();
}

bool Area::execute(double& outval, const Geometry& geom, const Geometry&) const{
    return false;
}
//----------------------------------------------------------
Centroid::Centroid()
{
}

GeometryMethod *Centroid::create() const{
        return new Centroid();
}

bool Centroid::execute(Geometry& outGeom, const Geometry& geom1, const Geometry &) const{
    return false;
}

//--------------------------------------------------------
ConvexHull::ConvexHull()
{
}

GeometryMethod *ConvexHull::create() const{
        return new Area();
}

bool ConvexHull::execute(Geometry& outGeom, const Geometry& geom, const Geometry &) const{
    return false;
}
