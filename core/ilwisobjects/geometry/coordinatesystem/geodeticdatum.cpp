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

#include <QSharedPointer>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

#include "kernel.h"
#include "module.h"
#include "connectorinterface.h"
#include "identity.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"

using namespace Ilwis;

const double rad2sec = 206264.80624709635515647335733078; //amount of arcseconds per radian

GeodeticDatum::GeodeticDatum(const QString& name) : Identity(name)
{
    _datumParams.resize(10,0);
    _datumParams[dmSCALE] = 1;
    _mode = dtMolodensky;
    _isValid = false;
    _ellWGS84.setEllipsoid(6378137.0, 298.257223563);
}

GeodeticDatum::GeodeticDatum(std::vector<double> &datumParameters, const IEllipsoid &ellips)
{
	_datumParams.resize(10,0);
	_mode = dtMolodensky;
	_isValid = false;
    if ( datumParameters.size() == 3) {
        set3TransformationParameters(datumParameters[0], datumParameters[1], datumParameters[2], ellips);
    } else if ( datumParameters.size() == 7) {
        set7TransformationParameters(datumParameters[0], datumParameters[1], datumParameters[2],
                datumParameters[3], datumParameters[4], datumParameters[5], datumParameters[6]);
    } else if ( datumParameters.size() == 10) {
        set10TransformationParameters(datumParameters[0], datumParameters[1], datumParameters[2],
                datumParameters[3], datumParameters[4], datumParameters[5],
                datumParameters[6], Coordinate(datumParameters[7], datumParameters[8], datumParameters[9]));
    }
    _ellWGS84.setEllipsoid(6378137.0, 298.257223563);
}

const bool GeodeticDatum::equal(const GeodeticDatum& datum) const {
    bool equal = true;
    switch (_mode) {
    case dtBadekas:
        equal = (_datumParams[dmCENTERXR] == datum._datumParams[dmCENTERXR]) && (_datumParams[dmCENTERYR] == datum._datumParams[dmCENTERYR]) && (_datumParams[dmCENTERZR] == datum._datumParams[dmCENTERZR]);
        // fall through
    case dtBursaWolf:
        equal = equal && (_datumParams[dmRX] == datum._datumParams[dmRX]) && (_datumParams[dmRY] == datum._datumParams[dmRY]) && (_datumParams[dmRZ] == datum._datumParams[dmRZ]) && (_datumParams[dmSCALE] == datum._datumParams[dmSCALE]);
        // fall through
    case dtMolodensky:
        equal = equal && (_datumParams[dmDX] == datum._datumParams[dmDX]) && (_datumParams[dmDY] == datum._datumParams[dmDY]) && (_datumParams[dmDZ] == datum._datumParams[dmDZ]);
    }
    return equal;
}

LatLon GeodeticDatum::llToWGS84(const LatLon& ll, const Ellipsoid& ell) const
{
    switch (_mode) {
    case dtMolodensky:
        return llhMolodensky(ll, ell,
        _ellWGS84.majorAxis() - ell.majorAxis(), _ellWGS84.flattening() - ell.flattening(),
            _datumParams[dmDX], _datumParams[dmDY], _datumParams[dmDZ]);
        break;
    case dtBursaWolf:
    {
        Coordinate ctsIn, ctsOut;
        Coordinate ctsPivot(0, 0, 0);
        ctsIn = ell.latlon2Coord(ll);
        ctsOut = ell.coord2coord(ctsIn, ctsPivot, _datumParams[dmDX], _datumParams[dmDY], _datumParams[dmDZ],
            _datumParams[dmRX], _datumParams[dmRY], _datumParams[dmRZ], _datumParams[dmSCALE]);
        return _ellWGS84.coord2latlon(ctsOut);
        break;
    }
    case dtBadekas:
    {
        Coordinate ctsIn, ctsOut;
        Coordinate ctsPivot(_datumParams[dmCENTERXR], _datumParams[dmCENTERYR], _datumParams[dmCENTERZR]);
        ctsIn = ell.latlon2Coord(ll);
        ctsOut = ell.coord2coord(ctsIn, ctsPivot, _datumParams[dmDX], _datumParams[dmDY], _datumParams[dmDZ],
            _datumParams[dmRX], _datumParams[dmRY], _datumParams[dmRZ], _datumParams[dmSCALE]);
        return _ellWGS84.coord2latlon(ctsOut);
        break;
    }
    }
}

LatLon GeodeticDatum::llFromWGS84(const LatLon& ll, const Ellipsoid& ell) const
{
    switch (_mode) {
    case dtMolodensky:
        return llhMolodensky(ll, _ellWGS84,
        ell.majorAxis() - _ellWGS84.majorAxis(), ell.flattening() - _ellWGS84.flattening(),
            -_datumParams[dmDX], -_datumParams[dmDY], -_datumParams[dmDZ]);
        break;
    case dtBursaWolf:
    {
        Coordinate ctsIn, ctsOut;
        Coordinate ctsPivot(0, 0, 0);
        ctsIn = _ellWGS84.latlon2Coord(ll);
        ctsOut = ell.coord2coord(ctsIn, ctsPivot, -_datumParams[dmDX], -_datumParams[dmDY], -_datumParams[dmDZ],
            -_datumParams[dmRX], -_datumParams[dmRY], -_datumParams[dmRZ], -_datumParams[dmSCALE]);
        return ell.coord2latlon(ctsOut);
        break;
    }
    case dtBadekas:
    {
        Coordinate ctsIn, ctsOut;
        Coordinate ctsPivot(_datumParams[dmCENTERXR], _datumParams[dmCENTERYR], _datumParams[dmCENTERZR]);
        ctsIn = _ellWGS84.latlon2Coord(ll);
        ctsOut = ell.coord2coord(ctsIn, ctsPivot, -_datumParams[dmDX], -_datumParams[dmDY], -_datumParams[dmDZ],
            -_datumParams[dmRX], -_datumParams[dmRY], -_datumParams[dmRZ], -_datumParams[dmSCALE]);
        return ell.coord2latlon(ctsOut);
        break;
    }
    }
}

// Molodensky datum transformation
// DMA Technical Report
// Department of Defense
// World Geodetic System 1984
// Its definition and relationships with local geodetic systems
// table 7.2, page 7-8/7-9
LatLon GeodeticDatum::llhMolodensky(const LatLon& llh,
											const Ellipsoid& ell,
											double da, double df,
											double dx, double dy, double dz) const
{
	double phi = llh.Phi();
	double lam = llh.Lambda();
	double h = llh.Height();
	double sinPhi = sin(phi);
	double cosPhi = cos(phi);
	double sinLam = sin(lam);
	double cosLam = cos(lam);
	double sin2Phi = sinPhi * sinPhi;

	// n = radius of curvature in the prime vertical
	double n = ell.majorAxis() / sqrt(1 - ell.excentricity2() * sin2Phi);
	// m = radius of curvature in the meridian
	double rTmp = 1 - ell.excentricity2() * sin2Phi;
	double m = ell.majorAxis() * (1 - ell.excentricity2()) / sqrt(rTmp * rTmp * rTmp);

	double dPhi, dLam, dh;
	dPhi = - dx * sinPhi * cosLam - dy * sinPhi * sinLam + dz * cosPhi
		+ da * (n * ell.excentricity2() * sinPhi * cosPhi) / ell.majorAxis()
		+ df * (m * ell.majorAxis() / ell.minorAxis() + n * ell.minorAxis() / ell.majorAxis()) * sinPhi * cosPhi;
	dPhi /= m + h;

	dLam = (-dx * sinLam + dy * cosLam) / ((n + h) * cosPhi);

	dh =   dx * cosPhi * cosLam + dy * cosPhi * sinLam + dz * sinPhi
		- da * ell.majorAxis() / n + df * n * sin2Phi * ell.minorAxis() / ell.majorAxis();

	phi += dPhi;
	lam += dLam;
	h += dh;
	LatLon llhRes;
	llhRes.Phi(phi);
	llhRes.Lambda(lam);
    llhRes.Height(h);
	return llhRes;
}

void GeodeticDatum::set3TransformationParameters(const double x, const double y, const double z, const IEllipsoid & ellips) {
    _datumParams[dmDX] = x;
    _datumParams[dmDY] = y;
    _datumParams[dmDZ] = z;
    _mode = dtMolodensky;
    code(QString("+towgs84=%1,%2,%3").arg(x).arg(y).arg(z));
    getFromInternal(ellips->code()); // table datums.csv isn't yet complete; we can only do this for 3 parameter molodensky
    _isValid = true;
}

void GeodeticDatum::set7TransformationParameters(const double x, const double y, const double z, const double rx, const double ry, const double rz, const double scale) {
    _datumParams[dmDX] = x;
    _datumParams[dmDY] = y;
    _datumParams[dmDZ] = z;
    _datumParams[dmRX] = rx / rad2sec;
    _datumParams[dmRY] = ry / rad2sec;
    _datumParams[dmRZ] = rz / rad2sec;
    _datumParams[dmSCALE] = scale / 1000000;
    _mode = dtBursaWolf;
    code(QString("%1,%2,%3,%4,%5").arg(code()).arg(rx).arg(ry).arg(rz).arg(scale));
    _isValid = true;
}

void GeodeticDatum::set10TransformationParameters(const double x, const double y, const double z, const double rx, const double ry, const double rz, const double scale, const Coordinate & center) {
    _datumParams[dmDX] = x;
    _datumParams[dmDY] = y;
    _datumParams[dmDZ] = z;
    _datumParams[dmRX] = rx / rad2sec;
    _datumParams[dmRY] = ry / rad2sec;
    _datumParams[dmRZ] = rz / rad2sec;
    _datumParams[dmSCALE] = scale / 1000000;
    _datumParams[dmCENTERXR] = center.x;
    _datumParams[dmCENTERYR] = center.y;
    _datumParams[dmCENTERZR] = center.z;
    _mode = dtBadekas;
    code(QString("%1,%2,%3,%4").arg(code()).arg( center.x).arg( center.y).arg( center.z));
    _isValid = true;
}

void GeodeticDatum::getFromInternal(const QString &ellips) {
    InternalDatabaseConnection db;
    QString query = "Select * from datum";
    if ( db.exec(query) ){
        while( db.next()){
             QSqlRecord rec = db.record();
             double dx = rec.field("dx").value().toDouble();
             double dy = rec.field("dy").value().toDouble();
             double dz = rec.field("dz").value().toDouble();
             QString ellipsoid = rec.field("ellipsoid").value().toString();
             // the combination of ellips-name, dx, dy and dz uniquely identifies the datum by name and area
             if ( ellipsoid.toLower() == ellips.toLower() && std::abs(_datumParams[dmDX] - dx) < 0.01 && std::abs(_datumParams[dmDY] - dy) < 0.01 && std::abs(_datumParams[dmDZ] - dz) < 0.01) {
                 _datumParams[dmDX] = dx;
                 _datumParams[dmDY] = dy;
                 _datumParams[dmDZ] = dz;
                 code(rec.field("code").value().toString());
                 name(rec.field("name").value().toString());
                 setArea(rec.field("area").value().toString());
                 setWktName(rec.field("wkt").value().toString());
                 setAuthority(rec.field("authority").value().toString());
                 return;
             }
        }
    }
}

GeodeticDatum *GeodeticDatum::clone() const
{
    GeodeticDatum *datum = new GeodeticDatum();
    datum->name(name())    ;
    datum->code(code());
    datum->setDescription(description());
    datum->_area = _area;
    datum->_authority = _authority;
    datum->_datumParams = _datumParams;
    datum->_mode = _mode;
    datum->_isValid = _isValid;
    datum->_wkt = _wkt;

    return datum;
}

double GeodeticDatum::parameter(DatumParameters parm) const {
    return _datumParams[parm];
}

bool GeodeticDatum::isValid() const
{
    bool ok = true;
    int count = 0;
    for(auto parm : _datumParams){
        if ( count != dmSCALE)
            ok |= parm != 0;
        ++count;
    }
    return ok && name() != sUNDEF;
}

GeodeticDatum::DatumTransformation GeodeticDatum::getTransformationMode() const {
    return _mode;
}

void GeodeticDatum::fromCode(const QString &gcode)
{
    InternalDatabaseConnection stmt;
    QString query = QString("Select * from datum where code='%1' or code='%2'").arg(gcode).arg('D' + gcode); // temporary workaround (only working for wgs84) because file datums.csv does not have datumnames that match the ones in epsg.pcs

    if (stmt.exec(query)) {
        if ( stmt.next()) {
            QString area = stmt.value(stmt.record().indexOf("area")).toString();
            QString geocode = stmt.value(stmt.record().indexOf("code")).toString();
            QString ellipsoid = stmt.value(stmt.record().indexOf("ellipsoid")).toString();
            double dx = stmt.value(stmt.record().indexOf("dx")).toDouble();
            double dy = stmt.value(stmt.record().indexOf("dy")).toDouble();
            double dz = stmt.value(stmt.record().indexOf("dz")).toDouble();
            setArea(area);
            code(geocode);
            setWktName(stmt.value(stmt.record().indexOf("wkt")).toString());
            IEllipsoid ellips;
            ellips.prepare("code=ellipsoid:" + ellipsoid);
            set3TransformationParameters(dx, dy, dz, ellips);
        } else {
            kernel()->issues()->log(TR("No datum for this code %1").arg(gcode));
        }
    } else {
        kernel()->issues()->logSql(stmt.lastError());
    }
}

void GeodeticDatum::setWktName(const QString &name)
{
    _wkt = name;
}

QString GeodeticDatum::toWKT(quint32 spaces) const
{
    if ( !isValid())
        return "";

    QString dat;
    QString indent = QString(" ").repeated(spaces);
    QString ending = spaces == 0 ? "" : "\n";
    if ( _wkt.size() > 0)
        dat = indent + "DATUM[\"" + _wkt + "," + ending;
    else{
        dat = indent + "DATUM[\" "+ (name() != sUNDEF ? name() : "UNKNOWN") + "\"," + ending;
        dat += indent + "[" + code().toUpper() + "]" + ending;
        dat.replace("+","");
        dat.replace("=","[");
    }

    return dat;
}

QString GeodeticDatum::area() const
{
    return _area;
}

void GeodeticDatum::setArea(const QString &v)
{
    _area = v;
}

QString GeodeticDatum::authority() const
{
    return _authority;
}

void GeodeticDatum::setAuthority(const QString& auth){
    _authority = auth;
}






