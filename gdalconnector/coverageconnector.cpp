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

#include <QLibrary>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include "kernel.h"
#include "coverage.h"
#include "module.h"

#include "projection.h"
#include "ilwisdata.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "columndefinition.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "coordinatesystem.h"
#include "proj4parameters.h"
#include "conventionalcoordinatesystem.h"
#include "table.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "gdalproxy.h"
#include "dataformat.h"
#include "gdalconnector.h"
#include "coverageconnector.h"



using namespace Ilwis;
using namespace Gdal;

CoverageConnector::CoverageConnector(const Resource& resource, bool load, const IOOptions &options) : GdalConnector(resource,load, options)
{
}

bool CoverageConnector::loadMetaData(Ilwis::IlwisObject *data,const IOOptions& options){

    if(!GdalConnector::loadMetaData(data, options))
        return false;

    Coverage *coverage = static_cast<Coverage *>(data);
    if ( coverage == nullptr)
        return false;

    ICoordinateSystem csy;
    QFileInfo fileinf = sourceRef().toLocalFile();
    if ( data->resource().hasProperty("coordinatesystem"))
        csy.prepare(data->resource()["coordinatesystem"].toString());
    if ( !csy.isValid()){
        OGRSpatialReferenceH srshandle = gdal()->srsHandle(_handle, data->name());
        if ( srshandle)
            csy = setObject<IConventionalCoordinateSystem>("coordinatesystem", QUrl::fromLocalFile(fileinf.absoluteFilePath()));
        else
            csy = setObject<ICoordinateSystem>("coordinatesystem", QUrl::fromLocalFile(fileinf.absoluteFilePath()));
    }

    if(!csy.isValid()) {
        // special handling for envi as the csy's are stored under the .hdr extension
        QString basepath = fileinf.absolutePath() + "/" + fileinf.baseName();
        QFileInfo enviinf = QString(basepath + ".hdr").toLower();
        if ( enviinf.exists()){
            csy = setObject<ICoordinateSystem>("coordinatesystem", QUrl::fromLocalFile(enviinf.absoluteFilePath()));
        }
        if (!csy.isValid()){
            QString resource = QString("code=csy:unknown");
            if (!csy.prepare(resource)) {
                kernel()->issues()->log(TR("Fallback to 'unknown' failed, corrupt system files definition"));
                return false;
            }
        }
    }
    coverage->coordinateSystem(csy);

    return true;
}

bool CoverageConnector::store(IlwisObject *obj, const IOOptions &options)
{
    return GdalConnector::store(obj, options);
}

OGRSpatialReferenceH CoverageConnector::createSRS(const ICoordinateSystem& coordsystem) const{
    if ( coordsystem.isValid() && hasType(coordsystem->ilwisType(), itCONVENTIONALCOORDSYSTEM)){
        IConventionalCoordinateSystem csy = coordsystem.as<ConventionalCoordinateSystem>();
        if ( csy->projection().isValid()){
            QString proj4def = csy->projection()->toProj4();
            OGRSpatialReferenceH srsH = gdal()->newSpatialRef(0);
            OGRErr errOgr = gdal()->importFromProj4(srsH, proj4def.toLocal8Bit());
            if ( errOgr != OGRERR_NONE) {
                return 0;
            }
            return srsH;
        }
    }
    return 0;
}



