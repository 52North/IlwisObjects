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

#include <QString>

#include "kernel.h"
#include "geometries.h"
#include "ilwisdata.h"
//#include "domain.h"
#include "coordinatesystem.h"
#include "conventionalcoordinatesystem.h"

using namespace Ilwis;

CoordinateSystem::CoordinateSystem()
{
}

CoordinateSystem::CoordinateSystem(const Resource& resource) : IlwisObject(resource)
{
}

Envelope CoordinateSystem::convertEnvelope(const ICoordinateSystem &sourceCs, const Envelope &envelope) const
{
    double rDX = (envelope.xlength() - 1.0)/10.0; // -1.0 as temporary workaround for Envelope.xlength() returning one too many.
    double rDY = (envelope.ylength() - 1.0)/10.0;
    int iX, iY;
    Coordinate cXY;
    Envelope env;
    for ( cXY.x = envelope.min_corner().x,iX = 0; iX <= 10; cXY += {rDX, 0}, ++iX) {
        for (cXY.y = envelope.min_corner().y,iY = 0; iY <= 10; cXY += {0,rDY} , ++iY ) {
            Coordinate crdNew = coord2coord(sourceCs, cXY);
            env += crdNew;
        }
    }
    return env;
}

bool CoordinateSystem::canConvertToLatLon() const
{
    return false;
}

bool CoordinateSystem::canConvertToCoordinate() const
{
    return false;
}

Coordinate CoordinateSystem::inverseCoordinateConversion(const CoordinateSystem& cs, const Coordinate &crd) const
{
    if (cs.isEqual(&cs))
      return crd;
    return Coordinate();
}

Envelope CoordinateSystem::envelope(bool tolatlon) const
{
    if ( tolatlon && !isLatLon() && code() != "csy:unknown"){
        LatLon c1 = coord2latlon(_envelope.min_corner());
        LatLon c2 = coord2latlon(_envelope.max_corner());
        return Envelope(c1,c2);
    }
    return _envelope;
}

Envelope  CoordinateSystem::latLonEnvelope(const ICoordinateSystem& cs, const Envelope& env) {
	Envelope result;
	if (!cs->isLatLon() && cs->code() != "csy:unknown") {
		LatLon c1 = cs->coord2latlon(env.min_corner());
		LatLon c2 = cs->coord2latlon(env.max_corner());
		return Envelope(c1, c2);
	}
	else if (cs->isLatLon()) {
		result = env;
	}
	return result;
}

void CoordinateSystem::envelope(const Envelope &env)
{
    _envelope = env;
}

void CoordinateSystem::copyTo(IlwisObject *obj)
{
    Locker<> lock(_mutex);
    IlwisObject::copyTo(obj);
    CoordinateSystem *csy = static_cast<CoordinateSystem *>(obj);
    csy->_envelope = _envelope;

}

bool CoordinateSystem::isUnknown() const
{
    return false;
}

bool CoordinateSystem::addCsyProperty(const ICoordinateSystem& csy, Resource& resource) {
	if (csy.isValid()) {
		if (csy->ilwisType() == itCONVENTIONALCOORDSYSTEM) {
			QString code("code=proj4:" + csy.as<ConventionalCoordinateSystem>()->toProj4());
			resource.addProperty("coordinatesystem", code, true);
			resource.setExtendedType(resource.extendedType() | itCOORDSYSTEM);
		}
		else if (csy->ilwisType() == itBOUNDSONLYCSY) {
			if (csy->code() == "unknown" || csy->code() == "code=csy:unknown") {
				resource.addProperty("coordinatesystem", "unknown");
			}else
				resource.addProperty("coordinatesystem", "boundsonly");

			resource.addProperty("envelope", csy->envelope().toString(), true);
		}
		else {
			QUrl url = csy->resourceRef().url(true);
			QFileInfo inf(url.toLocalFile());
			if (inf.exists()) {
				resource.addProperty("coordinatesystem", csy->resourceRef().url(true).toString(), true);
				return true;
			}
		}
	}
	return false;
}



