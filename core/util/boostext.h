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

#ifndef BOOSTEXT_H
#define BOOSTEXT_H

#include <cstddef>
#include <boost/geometry.hpp>
#include <boost/mpl/int.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/register/point.hpp>

#include <boost/geometry/io/wkt/wkt.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/register/box.hpp>

#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>
#include <boost/geometry/geometries/polygon.hpp>

BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(Coordinate2d, double, cs::cartesian, x, y, x, y)
BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(Pixel, qint32, cs::cartesian, x, y, x, y)
BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(Pixel_u, quint32, cs::cartesian, x, y, x, y)
BOOST_GEOMETRY_REGISTER_POINT_3D_GET_SET(Coordinate, double, cs::cartesian, x, y, z, x, y, z)
BOOST_GEOMETRY_REGISTER_POINT_3D_GET_SET(Voxel, qint32, cs::cartesian, x, y, z, x, y, z)
BOOST_GEOMETRY_REGISTER_POINT_3D_GET_SET(Voxel_u, quint32, cs::cartesian, x, y, z, x, y, z)

BOOST_GEOMETRY_REGISTER_LINESTRING(Ilwis::Line2D)
BOOST_GEOMETRY_REGISTER_LINESTRING(Ilwis::Line3D)

BOOST_GEOMETRY_REGISTER_BOX(Box2Dd,Coordinate2d, min_corner(), max_corner())
BOOST_GEOMETRY_REGISTER_BOX(Box2Di,Pixel, min_corner(), max_corner())
BOOST_GEOMETRY_REGISTER_BOX(Box2Dui,Pixel_u, min_corner(), max_corner())

BOOST_GEOMETRY_REGISTER_BOX(Box3Dd,Coordinate, min_corner(), max_corner())
BOOST_GEOMETRY_REGISTER_BOX(Box3Di,Voxel, min_corner(), max_corner())
BOOST_GEOMETRY_REGISTER_BOX(Box3Dui,Voxel_u, min_corner(), max_corner())




#endif // BOOSTEXT_H
