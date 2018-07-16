#include <QString>
#include <functional>

#include "kernel.h"
#include "ilwis.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "geos/geom/Coordinate.h"
#include "ilwiscoordinate.h"
#include "location.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "projectionimplementation.h"
#include "box.h"
#include "coordinatesystem.h"
#include "proj4parameters.h"
#include "conventionalcoordinatesystem.h"
#include "projectionimplementationinternal.h"
#include "proj4parameters.h"
#include "dutchrd.h"

using namespace Ilwis;
using namespace Internal;

#define  EPS10 1.e-10
//double  L0 = 5.387638889  * M_PI/180;

// Ref G. Strang van Hees, "Globale en lokale geodetische systemen"
// Nederl. Cie voor geodesie. Publicatie 30 2e Druk, Delft 1994
// pp. 18 - 21, pp. 26 - 28

Ilwis::Internal::DutchRD::DutchRD(const Ilwis::Resource &resource) : ProjectionImplementationInternal(resource)
{

}

DutchRD::~DutchRD()
{

}

void DutchRD::setCoordinateSystem(ConventionalCoordinateSystem *csy)
{
    ProjectionImplementationInternal::setCoordinateSystem(csy);
    _parameters[Projection::pvFALSEEASTING] = {155000};
	_parameters[Projection::pvFALSENORTHING] = {463000};
    _parameters[Projection::pvCENTRALPARALLEL] =  {(52 + 9.0/60.0 + 22.178/3600.0), itLATLON}; // lat0 / phi0: central (geodetic) latitude on ellipsoid
	_parameters[Projection::pvCENTRALMERIDIAN] =  {(5 + 23.0/60.0 + 15.5/3600.0), itLATLON}; // lon0 / lam0: central longitude
    _parameters[Projection::pvSCALE] = {0.9999079};
	_easting = 155000;
	_northing = 463000;
    _phi0 = _parameters[Projection::pvCENTRALPARALLEL]._value.toDouble() * M_PI / 180;
    _centralMeridian = _parameters[Projection::pvCENTRALMERIDIAN]._value.toDouble();
    _lam0 = _centralMeridian * M_PI / 180;
    _k0 = _parameters[Projection::pvSCALE]._value.toDouble();
    _rNgauss = 1.00047585668;
    _rMgauss = 0.003773953832;
    _B0 = 52.121097249 * M_PI/180;
    _maxis = _coordinateSystem->ellipsoid()->majorAxis();
    _ell_a = _maxis;
    _ell_e = _coordinateSystem->ellipsoid()->excentricity();
    _ell_e2 = _coordinateSystem->ellipsoid()->excentricity2();

	// from here on, the parameter values are not 'Dutch RD-specific' anymore
	// if phi0, lam0, x0, y0 and ell are chosen differently.
	
	///////////explicit computation of Sphere-radius rBOLSTRAAL:
    _rBOLSTRAAL = 6382644.571;
	double rWortel = sqrt(1 - _ell_e2 * sin(_phi0) * sin(_phi0));
	double rN0  = _ell_a / rWortel; //kromtestraal west-oost in Amersfoort 
	double rW3 = rWortel * rWortel * rWortel;
	double rM0  = _ell_a * (1 - _ell_e2) / rW3; //kromtestraal nrd-zd in Amersfoort (phi0,lam0)
	_rBOLSTRAAL = sqrt(rM0 * rN0);
	///////////explicit computation of Gauss coefficients:
	double e2prime = _ell_e2 / (1 - _ell_e2);
	double cos4Phi0 = cos(_phi0)*cos(_phi0)*cos(_phi0)*cos(_phi0);
	_rNgauss = sqrt(1 + e2prime * cos4Phi0);
	double eccSinPhi0 = _ell_e * sin(_phi0);
	// central isometric latitude on ellipsoid:
	double rQ0 = log(tan(M_PI_4 + _phi0/2)) - 0.5 * _ell_e * log((1 + eccSinPhi0)/(1 - eccSinPhi0));
	// central latitude on sphere: 
	_B0 = asin(sin(_phi0)/_rNgauss);
	// central isometric latitude on sphere: 
	double rW0 = log(tan(M_PI_4 + _B0/2)); 
	_rMgauss = rW0 - _rNgauss * rQ0;
// Ref G. Strang van Hees, "Globale en lokale geodetische systemen"
// Nederl. Cie voor geodesie. Publicatie 30, 3e Herziene Druk, Delft 1997
// pag 20
}

Coordinate DutchRD::ll2crd(const LatLon &pl) const
{
    // phi -> lat
    // lam -> lon
	Coordinate xy;
	double rQ, rQ1, rDQ, rW, rB, rDL;
	double ecc = _ell_e;
	double sinPsi, cosPsi, sinhalfPsi, coshalfPsi, tanhalfPsi;
    double eccsinPhi = ecc * sin(pl.lat().radians()); //(geodetic) latitude on ellipsoid
	rQ1 = log(tan(M_PI_4 + pl.lat().radians()/2));
	///rQ1 = atanh(sinPhi);
	rDQ = 0.5 * ecc * log(( 1 + eccsinPhi)/(1 - eccsinPhi));
	///rDQ = ecc * atanh( ecc * sinPhi);
	rQ = rQ1 - rDQ;										// isometric latitude on ellipsoid
	rW = _rNgauss * rQ + _rMgauss;			// isometric latitude on Schreiber sphere
	rB = 2 * atan(exp(rW)) - M_PI_2;	//  latitude on sphere
	rDL =  _rNgauss * pl.lon().radians(); ///rNgauss * (pl.Lam - lam0);
	double sin2halfPsi = sin((rB - _B0)/2) * sin((rB - _B0)/2) +
										 sin(rDL/2) * sin(rDL/2) * cos(rB) * cos(_B0);
	coshalfPsi = sqrt( 1 - sin2halfPsi);
	sinhalfPsi = sqrt(sin2halfPsi);
	tanhalfPsi = sinhalfPsi / coshalfPsi;
	sinPsi = 2 * sinhalfPsi * coshalfPsi; 
	if (abs(sinPsi) < EPS10) {
		xy.x = 0;
		xy.y = 0;
		return xy;
	}
	cosPsi = 1 - 2 * sin2halfPsi;
	double sinAlpha = sin(rDL) * cos(rB) / sinPsi;     // Alpha,Rho are polar coords in projection plane
	double cosAlpha = (sin(rB) - sin(_B0) * cosPsi) / (cos(_B0) * sinPsi); // origin Amersfoort
	double rRho = 2 * _k0 * _rBOLSTRAAL * tanhalfPsi / _ell_a; //           // at unit sphere scale
	xy.x = rRho * sinAlpha;
	xy.y = rRho * cosAlpha;
	return xy;
}

LatLon DutchRD::crd2ll(const Coordinate &xy) const
{
    // phi -> lat
    // lam -> lon
	LatLon pl;
	double rRho2 = xy.x * xy.x  + xy.y * xy.y;
	double rRho = sqrt(xy.x * xy.x  + xy.y * xy.y);// Hypoth function better ?
	if (rRho2 < EPS10) {
	  pl.lat(Angle(_phi0, true));
	  pl.lon(Angle(0.0, true));
	  return pl;        // point in projection origin (Amersfoort)
	}
	double sinAlpha = xy.x  / rRho;
	double cosAlpha = xy.y  / rRho;
	double rPsi = 2 * atan(rRho * _ell_a /(2 * _k0 * _rBOLSTRAAL)); // geocentric latitude
	double sinB = cosAlpha * cos(_B0) * sin(rPsi) + sin(_B0) * cos(rPsi);
	double rB = asin(sinB);								// latitude on Schreiber sphere
	double sinDL = sinAlpha * sin(rPsi) / cos(rB);
	double rW = log(tan(rB/2.0 + M_PI_4));// isometric latitude on sphere
	double rQ = (rW - _rMgauss) / _rNgauss;	// isometric latitude on ellipsoid
	double rPhiApprox = 2.0 * atan(exp(rQ)) - M_PI_2;
	double ecc = _ell_e;
	double rDQ = 0.5 * ecc * log(( 1 + ecc * sin(rPhiApprox))
												 /(1 - ecc * sin(rPhiApprox)));
	double rPhiNext = 2.0 * atan(exp(rQ + rDQ)) - M_PI_2;
	int iCount = 0;
	while (abs(rPhiNext - rPhiApprox) > EPS10) {
	  rPhiApprox = rPhiNext;
	  double rEccSin = ecc * sin(rPhiNext);
	  rDQ = 0.5 * ecc * log(( 1 + rEccSin )/(1 - rEccSin));
	  rPhiNext = 2.0 * atan(exp(rQ + rDQ)) - M_PI_2;
	  iCount++;
	  if (iCount > 10) return pl;
	}
	pl.lat(Angle(rPhiNext, true));							// (geodetic) latitude on ellipsoid (approximated)
	pl.lon(Angle(asin(sinDL)/_rNgauss, true)); /// + lam0;
	return pl;
}

bool DutchRD::canUse(const Ilwis::Resource &resource)
{
    QString prj = resource.code();
    if ( prj == sUNDEF)
        return false;
    if ( prj == "PRJDRD")
        return true;

    return false;
}

bool DutchRD::prepare(const QString &parms)
{
    return true;
}

ProjectionImplementation *DutchRD::clone()
{
    DutchRD *csy = new DutchRD();
    copyTo(csy);
    csy->_phi0 = _phi0;
    csy->_lam0 = _lam0;
    csy->_k0 = _k0;
    csy->_rBOLSTRAAL = _rBOLSTRAAL;
    csy->_rNgauss = _rNgauss;
    csy->_rMgauss = _rMgauss;
    csy->_B0 = _B0;
    csy->_ell_a = _ell_a;
    csy->_ell_e = _ell_e;
    csy->_ell_e2 = _ell_e2;
    return csy;
}

DutchRD::DutchRD()
{

}


