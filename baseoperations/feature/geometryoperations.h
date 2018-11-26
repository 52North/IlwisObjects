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

#ifndef WITHIN_H
#define WITHIN_H

namespace Ilwis{
class Within  : public GeometryMethod, public BoolFGeomGeom
{
public:
    Within();

    bool execute(bool& outBool,const Geometry& container, const Geometry& containee=Geometry()) const;
    GeometryMethod *create() const;
};

class Area  : public GeometryMethod, public DoubleFGeomGeom
{
public:
    Area();

    bool execute(double& outval,const Geometry& geom1,const Geometry& geom2=Geometry()) const;
    GeometryMethod *create() const;
};

class Centroid  : public GeometryMethod, public GeomFGeomGeom
{
public:
    Centroid();

    bool execute(Ilwis::Geometry &outGeom, const Geometry& geom1,const Geometry& geom2=Geometry()) const;
    GeometryMethod *create() const;
};

class ConvexHull  : public GeometryMethod, public GeomFGeomGeom
{
public:
    ConvexHull();

    bool execute(Ilwis::Geometry &outGeom, const Geometry& geom,const Geometry& geom2=Geometry()) const;
    GeometryMethod *create() const;
};

//------------------------------------------------
}

#endif // WITHIN_H
