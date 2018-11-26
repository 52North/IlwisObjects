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

#ifndef PROJECTIONIMPLEMENTATIONINTERNAL_H
#define PROJECTIONIMPLEMENTATIONINTERNAL_H

namespace Ilwis {
namespace Internal {
class ProjectionImplementationInternal : public ProjectionImplementation
{
public:
    ProjectionImplementationInternal(const Resource& resource);

    Coordinate latlon2coord(const LatLon&) const;
    LatLon coord2latlon(const Coordinate&) const;
    virtual void setCoordinateSystem(ConventionalCoordinateSystem *csy);
    QString toProj4() const;
    bool canConvertToLatLon() const;
    bool canConvertToCoordinate() const;
protected:
    void copyTo(ProjectionImplementation *prj);
    ProjectionImplementationInternal();
    virtual Coordinate pl2crd(const PhiLam&) const = 0;
    virtual PhiLam crd2pl(const Coordinate&) const = 0;

    double _easting;
    double _northing;
    double _maxis;
    //double _centralMeridian;
    double _phi0;
    double _lam0;

};
}
}

#endif // PROJECTIONIMPLEMENTATIONINTERNAL_H
