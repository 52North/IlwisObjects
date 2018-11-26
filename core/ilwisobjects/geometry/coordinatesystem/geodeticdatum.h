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

#ifndef DATUMDATA_H
#define DATUMDATA_H

#include "kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT GeodeticDatum : public Identity
{
public:
    enum DatumParameters{dmDX, dmDY, dmDZ, dmRX, dmRY, dmRZ, dmSCALE, dmCENTERXR, dmCENTERYR, dmCENTERZR};
    enum DatumTransformation{dtMolodensky, dtBursaWolf, dtBadekas};
    GeodeticDatum(const QString &name=sUNDEF);
    GeodeticDatum(std::vector<double>& datumParameters, const IEllipsoid &ellips);
    LatLon llToWGS84(const LatLon& ll, const Ellipsoid& ell) const;
    LatLon llFromWGS84(const LatLon& ll, const Ellipsoid& ell) const;
    const bool equal(const GeodeticDatum&) const;

    QString area() const;
    void setArea(const QString& v);
    QString authority() const;
    void setAuthority(const QString &auth);
    double parameter(DatumParameters parm) const;
    DatumTransformation getTransformationMode() const;
    bool isValid() const;
    void fromCode(const QString& code);
    void setWktName(const QString& name);
    QString toWKT(quint32 spaces=0) const;

    void set3TransformationParameters(const double x, const double y, const double z, const IEllipsoid &ellips);
    void set7TransformationParameters(const double x, const double y, const double z, const double rx, const double ry, const double rz, const double scale);
    void set10TransformationParameters(const double x, const double y, const double z, const double rx, const double ry, const double rz, const double scale, const Coordinate & center);

    GeodeticDatum *clone() const;
private:
    void getFromInternal(const QString & ellips);
    LatLon llhMolodensky(const LatLon& llh, const Ellipsoid& ell, double da, double df, double dx, double dy, double dz) const;
    Ellipsoid _ellWGS84;
    QString _area;
    QString _authority;
    std::vector<double> _datumParams;
    DatumTransformation _mode;
    bool _isValid;
    QString _wkt;
};

}

#endif // DATUMDATA_H
