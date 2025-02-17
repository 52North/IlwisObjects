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

#ifndef COORDINATESYSTEMVIALATLON_H
#define COORDINATESYSTEMVIALATLON_H

#include "kernel_global.h"

class Proj4Parameters;

namespace Ilwis {

class Ellipsoid;
class Projection;
class GeodeticDatum;


typedef IlwisData<Ellipsoid> IEllipsoid;
typedef IlwisData<Projection> IProjection;


class KERNELSHARED_EXPORT ConventionalCoordinateSystem : public CoordinateSystem
{
public:
    ConventionalCoordinateSystem();
    ConventionalCoordinateSystem(const Resource& resource);
    ~ConventionalCoordinateSystem();

    Coordinate coord2coord(const ICoordinateSystem &sourceCs, const Coordinate& crdSource) const;
    LatLon coord2latlon(const Coordinate &crdSource) const;
    Coordinate latlon2coord(const LatLon& ll) const;
    const std::unique_ptr<Ilwis::GeodeticDatum> &datum() const;
    void setDatum(Ilwis::GeodeticDatum *datum);
    IEllipsoid ellipsoid() const;
    void setEllipsoid(const IEllipsoid& ell);
    void setProjection(const IProjection& proj);
    IProjection projection() const;
    virtual bool isLatLon() const;
    bool prepare(const QString& parms);
    bool prepare(const IOOptions& options=IOOptions());
    IlwisTypes ilwisType() const;
    bool isValid() const;
    bool isEqual(const Ilwis::IlwisObject *obj) const;
    bool isUnknown() const;
    bool canConvertToLatLon() const;
    bool canConvertToCoordinate() const;
    QString toWKT(quint32 spaces=0) const;
    QString toProj4() const;
    QString toEpsg() const;
    QString unit() const;
    void unit(const QString &unit);
    bool isCompatibleWith(const IlwisObject *obj, bool strict=false) const;
    IlwisObject *clone();
private:
    void copyTo(IlwisObject *obj);
    QString extractProjection(const Proj4Parameters & proj4) const;

    IEllipsoid _ellipsoid;
    IProjection _projection;
    std::unique_ptr<GeodeticDatum> _datum;
    QString _unit;
};

typedef IlwisData<ConventionalCoordinateSystem> IConventionalCoordinateSystem;

}
Q_DECLARE_METATYPE(Ilwis::IConventionalCoordinateSystem);

#endif // COORDINATESYSTEMVIALATLON_H
