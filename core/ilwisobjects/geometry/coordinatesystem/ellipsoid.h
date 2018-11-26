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

#ifndef ELLIPSOID_H
#define ELLIPSOID_H

#include "kernel_global.h"

namespace Ilwis {
class Ellipsoid;

typedef IlwisData<Ilwis::Ellipsoid> IEllipsoid;
#define WGS84RESOURCE QString("ilwis://system/ellipsoids/WE")

class KERNELSHARED_EXPORT Ellipsoid : public IlwisObject
{
public:
    Ellipsoid();
    Ellipsoid(const Resource &resource);
    virtual ~Ellipsoid();

    bool isSpherical() const;
    bool isEqual(const IEllipsoid &ell) const;
    bool isValid() const;
    double distance(const LatLon& begin, const LatLon &end) const;
    double azimuth(const LatLon &begin, const LatLon &end) const;
    double majorAxis() const;
    double minorAxis() const;
    double flattening() const;
    double excentricity() const;
    double excentricity2() const;
    void setEllipsoid(double a, double invf);
    QString authority() const;
    void setAuthority(const QString &auth);
    QString toProj4() const;
    QString toWKT(quint32 spaces=0) const;
    QString wktShort() const;
    IlwisTypes ilwisType() const;
    void setWKTName(const QString& wkt);
    static QString ellipsoidCode2Name(const QString& code);

    LatLon latlon2Coord(const IEllipsoid &sourceEllipsoide, const LatLon &sourceLatLon) const;
    Coordinate latlon2Coord(const LatLon &sourceLatLon) const;
    LatLon coord2latlon(const Coordinate &crd) const;
    Coordinate coord2coord(const Coordinate& ctsIn, const Coordinate& ctsPivot,
                                                            double tx, double ty, double tz,
                                                            double Rx, double Ry, double Rz,
                                                            double s) const;

    static const IEllipsoid wgs84ellipsoid;

    void fromInternal(const QSqlRecord &rec);
    IlwisObject* clone();

private:
    void copyTo(IlwisObject *e);
    Ellipsoid(double a, double invf);

    double _flattening;
    double _majorAxis;
    double _minoraxis;
    double _excentricity;
    QString _authority;
    QString _wkt;

    static std::map<QString, QString> _ellcode2Name;
};

}

Q_DECLARE_METATYPE(Ilwis::IEllipsoid)

#endif // ELLIPSOID_H
