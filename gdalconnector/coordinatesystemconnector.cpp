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

#include <QUrl>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QStringList>

#include "kernel.h"
#include "geometries.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
//#include "domain.h"
#include "coordinatesystem.h"
#include "proj4parameters.h"
#include "conventionalcoordinatesystem.h"
#include "boundsonlycoordinatesystem.h"
#include "gdalproxy.h"
#include "dataformat.h"
#include "gdalconnector.h"
#include "coordinatesystemconnector.h"
#include "ilwiscontext.h"

using namespace Ilwis;
using namespace Gdal;

ConnectorInterface *CoordinateSystemConnector::create(const Resource &resource, bool load, const IOOptions &options){
    return new CoordinateSystemConnector(resource, load, options);
}

IlwisObject *CoordinateSystemConnector::create() const{
    IlwisObject *object = 0;
    if ( type() == itCONVENTIONALCOORDSYSTEM)
        object = new ConventionalCoordinateSystem(_resource);
	if (hasType(type(), itBOUNDSONLYCSY))
		object = new BoundsOnlyCoordinateSystem(_resource);
    return object;
}

CoordinateSystemConnector::CoordinateSystemConnector(const Resource &resource, bool load, const IOOptions &options) : GdalConnector(resource, load, options)
{

}



void CoordinateSystemConnector::extractUserDefinedEllipsoid(ConventionalCoordinateSystem *csyp, OGRSpatialReferenceH srshandle)
{
    IEllipsoid ellipsoid;
    QString axis(gdal()->getAttributeValue(srshandle,"SPHEROID",1));
    QString flat(gdal()->getAttributeValue(srshandle,"SPHEROID",2));
    bool ok1, ok2;
    double laxis = axis.toDouble(&ok1);
    double flattening = flat.toDouble(&ok2);
    if ( ok1 && ok2){
        ellipsoid.prepare();
        ellipsoid->name("unnamend");
        ellipsoid->setEllipsoid(laxis, 1.0/flattening);
        csyp->setEllipsoid(ellipsoid);
    }
}

std::vector<double> CoordinateSystemConnector::getShifts(QString & filename, QString & datumName) const {
    std::vector<double> result;
    auto basePath = context()->ilwisFolder().absoluteFilePath() + "/extensions/gdalconnector/resources";
    QFileInfo fileInfo(basePath + "/" + filename);
    if ( !fileInfo.exists()) {
        kernel()->issues()->log(TR(ERR_MISSING_SYSTEM_FILE_1).arg(filename), IssueObject::itCritical);
        return result;
    }

    QFile file(fileInfo.absoluteFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        kernel()->issues()->log(TR(ERR_MISSING_SYSTEM_FILE_1).arg(filename), IssueObject::itCritical);
        return result;
    }

    QTextStream txtFile(&file);
	txtFile.setCodec("UTF-8");
    bool skip = true;
    while (!txtFile.atEnd()) {
        QString line = txtFile.readLine();
        if (skip) { // first line is header
            skip = false;
            continue;
        }
        QStringList parts = line.split(',');
        // skip non-applcable lines
        if (parts.size() < 21) {
            continue;
        }
        if (parts[0].isEmpty() || parts[0].startsWith("#")) {
            continue;
        }
        if (parts[1].trimmed() == datumName || parts[3].trimmed() == datumName) {
            for (int i = 14; i < 21; ++i)
                result.push_back(parts[i].trimmed().toDouble());
        }
    }
    return result;
}

bool CoordinateSystemConnector::loadMetaData(IlwisObject *data, const IOOptions &options){

	if (data->code() == "csy:unknown") {
		return true;
	}
    if (!GdalConnector::loadMetaData(data, options))
        return false;
    bool ret = true;
    OGRSpatialReferenceH srshandle = gdal()->srsHandle(_handle, data->name());
    if (srshandle){
        CoordinateSystem *csy = static_cast<CoordinateSystem *>(data);

        if ( type() == itCONVENTIONALCOORDSYSTEM) {
            ConventionalCoordinateSystem *csyp = static_cast<ConventionalCoordinateSystem *>(csy);
            QString ellipsoidName(gdal()->getAttributeValue(srshandle,"SPHEROID",0));
            char *wkt = new char[10000];
            gdal()->exportToPrettyWkt(srshandle,&wkt,TRUE);
            IEllipsoid ellipsoid;
            if ( (ellipsoidName.compare("unnamed",Qt::CaseInsensitive) != 0) && (ellipsoidName.compare("unknown",Qt::CaseInsensitive) != 0)) {
                ellipsoid.prepare("code=wkt:" + ellipsoidName);
                if ( ellipsoid.isValid())
                    csyp->setEllipsoid(ellipsoid);
                else
                 extractUserDefinedEllipsoid(csyp, srshandle);
            }else {
                extractUserDefinedEllipsoid(csyp, srshandle);
            }
            QString datumName(gdal()->getAttributeValue(srshandle,"Datum",0));
           // if ( datumName == "WGS_1984")
            //    csy->prepare("+proj=longlat +ellps=WGS84 +datum=WGS84");
           
            GeodeticDatum *datum = new GeodeticDatum(datumName);
auto overrides = QString("gcs.override.csv");
            std::vector<double> shifts = getShifts(overrides, datumName);
            if (shifts.size() == 0){
                auto gcs = QString("gcs.csv");
                shifts = getShifts(gcs, datumName);
            }if (shifts.size() >= 3) {
                if (shifts.size() >= 7)
                    datum->set7TransformationParameters(shifts[0], shifts[1], shifts[2], shifts[3], shifts[4], shifts[5], shifts[6]);
                else
                    datum->set3TransformationParameters(shifts[0], shifts[1], shifts[2], ellipsoid);
            }
            //datum.prepare("wkt=" + datumName);
            if ( datum->isValid())
                csyp->setDatum(datum);

            QString projectionName(gdal()->getAttributeValue(srshandle,"Projection",0));
            if ( projectionName != "") {
                IProjection projection;
                IOOptions opt = options;
                if (_resource.code().indexOf("code=") == 0){
                    opt << IOOptions::Option("proj4", _resource.code().mid(6));
                }else
                    opt << IOOptions::Option("proj4", _resource.code());

                projection.prepare("code=wkt:" + projectionName, opt);
                if ( projection.isValid()) {
                    setProjectionParameter(srshandle, "false_easting", Projection::pvFALSEEASTING, projection);
                    setProjectionParameter(srshandle, "false_northing", Projection::pvFALSENORTHING, projection);
                    setProjectionParameter(srshandle, "scale_factor", Projection::pvSCALE, projection);
                    setProjectionParameter(srshandle, "central_meridian", Projection::pvCENTRALMERIDIAN, projection);
                    setProjectionParameter(srshandle, "latitude_of_origin", Projection::pvCENTRALPARALLEL, projection);
                    setProjectionParameter(srshandle, "standard_parallel_1", Projection::pvSTANDARDPARALLEL1, projection);
                    setProjectionParameter(srshandle, "standard_parallel_2", Projection::pvSTANDARDPARALLEL2, projection);
                    setProjectionParameter(srshandle, "zone", Projection::pvZONE, projection);
                    csyp->setProjection(projection);
                    projection->setCoordinateSystem(csyp);
                }
            }else // fallback, not all gdal wkt are correct
            {
                char *proj4;
                gdal()->export2Proj4(srshandle, &proj4);
                QString sproj4 = proj4;
                if ( proj4){
                    gdal()->free(proj4);
                    csy->prepare(sproj4);
                }
            }
           
        Envelope env = gdal()->envelope(_handle, 0);
        if ( env.isValid() && !env.isNull()){
            csy->envelope(env);
        }

        }
    }else{
		CoordinateSystem *csy = dynamic_cast<CoordinateSystem *>(data);
		if ( !csy || csy->ilwisType() != itBOUNDSONLYCSY)
			ret = ERROR2(ERR_INVALID_PROPERTY_FOR_2,"OGRSpatialReference",data->name());
		csy->code("csy:unknown");
    }
    gdal()->releaseSrsHandle(_handle, srshandle, data->name());
    QFileInfo fileinf(sourceRef().toLocalFile());
    gdal()->closeFile(fileinf.absoluteFilePath(), data->id());
    return ret;
}

void CoordinateSystemConnector::setProjectionParameter(OGRSpatialReferenceH handle, const char *wkt_name, Projection::ProjectionParamValue parmType, IProjection& projection){
    OGRErr err;
    double value = gdal()->getProjectionParm(handle, wkt_name, rUNDEF, &err);
    if ( err == OGRERR_NONE && value != rUNDEF) {
        projection->setParameter(parmType, value);
    }
}



