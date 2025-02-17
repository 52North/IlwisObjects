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
#include <regex>

#include "kernel.h"
#include "geometries.h"
#include "ilwisdata.h"
//#include "domain.h"
#include "coordinatesystem.h"
#include "projection.h"
#include "ellipsoid.h"
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
			crdNew.z = rUNDEF; // prevent env unintentionally expanding to 3D, as this has side-effects; many applications assume a 2D envelope will be returned; For now this function only handles 2D envelopes
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
			auto ccs = csy.as<ConventionalCoordinateSystem>();
			auto code = ccs->toProj4();
			if (code != sUNDEF) {
				code = QString("code=proj4:" + code);
			}
			else {
				if (ccs->projection().isValid() && ccs->projection()->code() == "PRJPC") // special case as plate carree is not supported by proj4 and deprecated in epsg
					code = "code=epsg:32662";
			}
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

void setProjectionParameter(IProjection& proj, Projection::ProjectionParamValue pvType, const std::string& wkt, const std::regex& regex ){
    std::smatch matches;
    auto value = std::regex_search(wkt, matches, regex) ? QString::fromStdString(matches[1]) : sUNDEF;
    if ( value != sUNDEF){
        proj->setParameter(pvType, value.toDouble());
    }
}

ICoordinateSystem CoordinateSystem::fromWKT(const QString& wkt){
    std::regex projcsRegex(R"(PROJCS\[\s*\"([^\"]+)\"\s*,)");
    std::regex geocsRegex(R"(GEOGCS\[\s*\"([^\"]+)\"\s*,)");
    std::regex spheroidRegex(R"(SPHEROID\[\s*\"([^\"]+)\"\s*,\s*([\d\.]+)\s*,\s*([\d\.]+))");
    std::regex projectionRegex(R"(PROJECTION\[\s*\"([^\"]+)\"\s*\])");
    std::regex centralMeridianRegex(R"(PARAMETER\[\s*\"central_meridian\"\s*,\s*([\d\.\-]+))");
    std::regex scaleFactorRegex(R"(PARAMETER\[\s*\"scale_factor\"\s*,\s*([\d\.\-]+))");
    std::regex falseEastingRegex(R"(PARAMETER\[\s*\"false_easting\"\s*,\s*([\d\.\-]+))");
    std::regex falseNorthingRegex(R"(PARAMETER\[\s*\"false_northing\"\s*,\s*([\d\.\-]+))");
    std::regex latitude_of_originRegex(R"(PARAMETER\[\s*\"latitude_of_origin\"\s*,\s*([\d\.\-]+))");
    std::regex standardPar1(R"(PARAMETER\[\s*\"standard_parallel_1 \"\s*,\s*([\d\.\-]+))");
    std::regex standardPar2(R"(PARAMETER\[\s*\"standard_parallel_2 \"\s*,\s*([\d\.\-]+))");
    std::regex azimuth(R"(PARAMETER\[\s*\"azimuth \"\s*,\s*([\d\.\-]+))");
    std::smatch matches;

    InternalDatabaseConnection db;
    auto swkt = wkt.toStdString();
    if (std::regex_search(swkt, matches, projcsRegex)) {
        QString name = QString::fromStdString(matches[1]);

        QString query(QString("select proj_params from projectedcsy where name='%1'").arg(name));
        if ( db.exec(query)) {
            if (db.next()) {
                QSqlRecord rec = db.record();
                auto projParms = rec.value("proj_params").toString();
                ICoordinateSystem csy("proj4=" + projParms);
                if ( csy.isValid())
                    return csy;
            }
        }
    }else{
            if (std::regex_search(swkt, matches, geocsRegex)) {
                auto wkt_name = QString::fromStdString(matches[1]);
                if ( wkt_name == "WGS 84"){
                    ICoordinateSystem csy("code=epsg:4326");
                    return csy;
                }
            }
            IEllipsoid ell;
            if (std::regex_search(swkt, matches, spheroidRegex)) {
                QString name = QString::fromStdString(matches[1]);
                QString query(QString("select code from ellipsoid where wkt='%1' COLLATE NOCASE").arg(name));
                if ( db.exec(query)) {
                    if (db.next()) {
                        QSqlRecord rec = db.record();
                        auto code = rec.value("code").toString();
                        ell.prepare("code=" + code);
                    }
                }
                if (! ell.isValid()){
                    QString sma = QString::fromStdString(matches[2]);
                    QString rflat = QString::fromStdString(matches[3]);
                    ell.prepare();
                    ell->setEllipsoid(sma.toDouble(),rflat.toDouble());
                }
            }

            IProjection proj;
            if (std::regex_search(swkt, matches, projcsRegex)) {
                auto wkt_name = QString::fromStdString(matches[1]);
                QString query(QString("select code from projection where wkt='%1' COLLATE NOCASE").arg(wkt_name));
                if ( db.exec(query)) {
                    if (db.next()) {
                        QSqlRecord rec = db.record();
                        IProjection prj;
                        proj.prepare("code=" + rec.value("code").toString());
                        // TODO: this probably has to be extended with the rest of the parameters but for the moment we assume that first query works
                    }
                }
                setProjectionParameter(proj, Projection::pvCENTRALMERIDIAN, wkt.toStdString(), centralMeridianRegex);
                setProjectionParameter(proj, Projection::pvSCALE, wkt.toStdString(), scaleFactorRegex);
                setProjectionParameter(proj, Projection::pvFALSEEASTING, wkt.toStdString(), falseEastingRegex);
                setProjectionParameter(proj, Projection::pvFALSENORTHING, wkt.toStdString(), falseNorthingRegex);
                setProjectionParameter(proj, Projection::pvLATITUDEOFTRUESCALE, wkt.toStdString(), latitude_of_originRegex);
                setProjectionParameter(proj, Projection::pvSTANDARDPARALLEL1, wkt.toStdString(), standardPar1);
                setProjectionParameter(proj, Projection::pvSTANDARDPARALLEL2, wkt.toStdString(), standardPar2);
                setProjectionParameter(proj, Projection::pvAZIMYAXIS, wkt.toStdString(), azimuth);
            }
            if ( ell.isValid() && proj.isValid()){
                IConventionalCoordinateSystem csy;
                csy.prepare();
                csy->setEllipsoid(ell);
                csy->setProjection(proj);
                return csy;
            }
    }

    return ICoordinateSystem();
}
