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

#ifndef COORDINATE_H
#define COORDINATE_H

#include "ilwis.h"

namespace geos{
    namespace geom{
    class CoordinateSequence;
    class Coordinate;
    }
}

namespace Ilwis {
class KERNELSHARED_EXPORT Coordinate : public geos::geom::Coordinate{
public:
    Coordinate();
    Coordinate(double px, double py, double pz=rUNDEF);
    Coordinate(const geos::geom::Coordinate& crd);
    Coordinate(const Ilwis::Coordinate& crd);
    Coordinate(const std::vector<double> &v);
    Coordinate(const QVector3D& crd);
    virtual ~Coordinate();

    static double undefined() { return  rUNDEF; }
    static quint64 valuetype() { return itDOUBLE;}
    Coordinate& operator=(const geos::geom::Coordinate&);
    bool isValid() const;
    bool is0() const;
    bool is3D() const;
    operator std::vector<double>();
    Ilwis::Coordinate &operator =(const Ilwis::Coordinate &p2);
    Ilwis::Coordinate &operator =(const Ilwis::Coordinate&& p2);
    Ilwis::Coordinate &operator =(const QVector3D& p2);
    Coordinate &operator +=(const std::vector<double> &vec);
    Coordinate &operator -=(const std::vector<double> &vec);
    double distance(const Coordinate &crd);
    Ilwis::Coordinate &operator *=(const std::vector<double> &vec);
    Ilwis::Coordinate &operator *=(double v);
    Ilwis::Coordinate &operator /=(double v);
    bool operator ==(const Ilwis::Coordinate &pnt) const;
    bool operator ==(const geos::geom::Coordinate &pnt) const;
    bool operator !=(const Ilwis::Coordinate &pnt);
    bool isEqual(const Ilwis::Coordinate& crd, double delta = rUNDEF);
    virtual QString toString(int decimals=2, bool use3D = false) const;


    QVariantMap toMap() const{
        QVariantMap mp;
        mp["x"] = x;
        mp["y"] = y;
        mp["z"] = z;
        return mp;
    }
};

struct KERNELSHARED_EXPORT PhiLam  // Phi and Lam in radians; philam is always in radians
{
	double Phi, Lam;
	PhiLam(): Phi(rUNDEF), Lam(rUNDEF) {}
	bool fUndef() { return Phi == rUNDEF || Lam == rUNDEF; }
	void AdjustLon();
};

class KERNELSHARED_EXPORT LatLon : public Ilwis::Coordinate { // lat == y, lon == x; latlon is always in degrees
public:
    LatLon();
    LatLon(const double latd, const double lond, const double height = 0);
    LatLon(const QString& lat, const QString& lon);
    double Phi() const    { return M_PI / 180 * y; }
    double Lambda() const { return M_PI / 180 * x; }
    void Phi(double phi)       { y = phi * 180 / M_PI; }
    void Lambda(double lambda) { x = lambda * 180 / M_PI; }
    void Height(double height) { z = height; }
    double Lat() const { return y; }
    double Lon() const { return x; }
    double Height() const { return z; }
    virtual QString toString(int decimals=2, bool use3D = false) const;
};

#define llUNDEF  Ilwis::LatLon(rUNDEF, rUNDEF)

KERNELSHARED_EXPORT std::vector<double> operator-(const Ilwis::Coordinate& p1, const Ilwis::Coordinate& p2);
KERNELSHARED_EXPORT Ilwis::Coordinate operator+(const Ilwis::Coordinate& p1, const std::vector<double>& vec);
KERNELSHARED_EXPORT Ilwis::Coordinate operator-(const Ilwis::Coordinate& p1, const std::vector<double>& vec);
KERNELSHARED_EXPORT Ilwis::Coordinate operator*(const Ilwis::Coordinate& p1, double v);
KERNELSHARED_EXPORT Ilwis::Coordinate operator/(const Ilwis::Coordinate& p1, double v);
}

#define crdUNDEF Ilwis::Coordinate(rUNDEF, rUNDEF, rUNDEF)
typedef std::unique_ptr<geos::geom::CoordinateSequence *> UPCoordinates;



Q_DECLARE_METATYPE(Ilwis::Coordinate)

#endif // COORDINATE_H

