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

#include "kernel.h"
#include "geometries.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "ilwiscoordinate.h"
#include "georeference.h"
#include "georefimplementation.h" 
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"

using namespace Ilwis;

CornersGeoReference::CornersGeoReference() : SimpelGeoReference("corners")
{

}

GeoRefImplementation *CornersGeoReference::create()
{
    return new CornersGeoReference();
}

void CornersGeoReference::copyTo(GeoRefImplementation *impl)
{
    SimpelGeoReference::copyTo(impl);
    static_cast<CornersGeoReference *>(impl)->_internalEnvelope = _internalEnvelope;
}

void CornersGeoReference::internalEnvelope(const Envelope &env)
{
	_internalEnvelope = env;
}

int CornersGeoReference::compute()
{
    bool a = size().isNull();
    bool b = _internalEnvelope.isValid();
    if (a || !b)
        return 0;

    _a12 = _a21 = 0;
    std::vector<double> vec = _internalEnvelope.max_corner() - _internalEnvelope.min_corner();

    bool deltaxSmall = (std::abs(vec[0]) < 0.0000001);
    bool deltaySmall = (std::abs(vec[1]) < 0.0000001);
    if ( deltaxSmall || deltaySmall) {
        return 0;
    }
	if (!_centerOfPixel) { // corners of corner pixels
		_a11 = size().xsize() / vec[0];
		double tempy = size().ysize();
		_a22 = -tempy / vec[1];
		_b1 = -_a11 * _internalEnvelope.min_corner().x;
		_b2 = -_a22 * _internalEnvelope.max_corner().y;
	}
	else { // center of corner pixels
		_a11 = (size().xsize() - 1) / vec[0];
		double v1 = size().ysize() - 1;
		double v2 = vec[1];
		double v3 = -v1 / v2;
		_a22 = v3;
		_b1 = 0.5 - _a11 * _internalEnvelope.min_corner().x;
		_b2 = 0.5 - _a22 * _internalEnvelope.max_corner().y;
	}
    _det = _a11 * _a22;


    return 1;
}

Envelope CornersGeoReference::internalEnvelope() const
{
    return _internalEnvelope;
}

GeoRefImplementation *CornersGeoReference::clone()
{
    GeoRefImplementation *impl = new CornersGeoReference();
    copyTo(impl);
    return impl;
}

QString CornersGeoReference::grfType() const {
	return CornersGeoReference::typeName();
}

QString CornersGeoReference::typeName()
{
    return "corners";
}

bool CornersGeoReference::isCompatible(const IGeoReference &georefOther) const {
    if (size() != georefOther->size())
        return false;
    if (!coordinateSystem()->isCompatibleWith(georefOther->coordinateSystem().ptr()))
        return false;
    Ilwis::Box<Ilwis::Coordinate> box = envelope();
    Ilwis::Box<Ilwis::Coordinate> boxOther = georefOther->envelope();
    double delta = coordinateSystem()->isLatLon() ? 0.000001 : 0.01;
    
    return box.equals(boxOther, delta);
}

QString CornersGeoReference::toCode() const
{
    // newGeoRefStr = ("code=georef:type=corners,csy=epsg:2050,envelope=" +
     //                       str(targetEnv) + ",gridsize=" + str(sz.xsize) + " " + str(sz.ysize) +
     //                       ",name=grf1")
     QString csyCode = coordinateSystem()->code();
     QString sizeS = size().toString();
     QString envS = coordinateSystem()->envelope().toString();
     QString outCode = QString("code=georef:type=corners,csy=%1,envelope=%2,gridsize=%3").arg(csyCode).arg(envS).arg(sizeS);
    return outCode;
}



