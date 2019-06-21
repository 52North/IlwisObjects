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
#include <QUrl>
#include <QFileInfo>
#include <QDir>

#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "inifile.h"
#include "coordinatesystem.h"
#include "ilwis3connector.h"
#include "rawconverter.h"
#include "georeference.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "controlpoint.h"
#include "ctpgeoreference.h"
#include "eigen3/Eigen/LU"
#include "mathhelper.h"
#include "planarctpgeoreference.h"
#include "georefconnector.h"
#include "binaryilwis3table.h"
#include "factory.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "abstractfactory.h"
#include "georefimplementationfactory.h"

using namespace Ilwis;
using namespace Ilwis3;

ConnectorInterface *GeorefConnector::create(const Resource &resource, bool load, const IOOptions &options) {
    return new GeorefConnector(resource, load, options);

}


GeorefConnector::GeorefConnector(const Resource &resource, bool load, const IOOptions &options) : Ilwis3Connector(resource, load, options) {

}


bool GeorefConnector::loadGeoref(const IniFile &odf, IlwisObject *data ) {
    QString type = odf.value("GeoRef","Type");
    if ( type == "?") {
        kernel()->issues()->log(TR("Invalid Georef section in %1").arg(odf.url()));
        return false;
    }
    bool ok1, ok2;
    quint32 lines = odf.value("GeoRef","Lines").toInt(&ok1);
    quint32 columns = odf.value("GeoRef","Columns").toInt(&ok2);
    if ( !(ok1 & ok2)) {
        // no consistency in how lines/columns are spelled ( casing). Mostely the above casing is used, but not always
        // so we try again
        lines = odf.value("GeoRef","lines").toInt(&ok1);
        columns = odf.value("GeoRef","columns").toInt(&ok2);
        if ( !(ok1 & ok2))
            return ERROR2(ERR_INVALID_PROPERTY_FOR_2,"Lines/Columns","Georeference");
    }

    GeoReference *grf = static_cast<GeoReference *>(data);
    grf->size(Size<>(columns, lines,1));
    if ( type == "GeoRefCorners") {
        return loadGeorefCorners(odf, data);
    } else if ( type == "GeoRefSubMap") {
        QString name = odf.value("GeoRefSubMap","GeoRef");
        QStringList rcStart = odf.value("GeoRefSubMap", "Start").split(' ');
        bool ok = rcStart.size() == 2;
        Pixeld pixStart;
        bool ok1 = false, ok2 = false;
        if (ok)
            pixStart = Pixeld(rcStart[1].toInt(&ok1), rcStart[0].toInt(&ok2)); // Row = [0], Col = [1]
        if ( !(ok && ok1 & ok2))
            return ERROR2(ERR_INVALID_PROPERTY_FOR_2,"Missing section Start Lines/Columns","Georeference SubMap");
        QUrl resource = mastercatalog()->name2url(name, itGEOREF);
        IniFile odfParent;
        odfParent.setIniFile(resource.toLocalFile());
        ok = loadGeoref(odfParent,data); // this call "chains" all the way to the "root" georeference, which is either Corners or CTP
        if (!ok)
            return false;
        if (grf->grfType<CornersGeoReference>()) { // relationship rc/crd is linear: "translate" the envelope of the georeference
            Coordinate crd1 = grf->pixel2Coord(pixStart); // use the "parent" georef to compute
            pixStart += Pixeld(columns, lines);
            Coordinate crd2 = grf->pixel2Coord(pixStart);
            Envelope env(crd1, crd2);
			QSharedPointer< CornersGeoReference> spGrf = grf->as< CornersGeoReference>();
			spGrf->internalEnvelope(env);
        } else if (grf->grfType<CTPGeoReference>()) { // relationship rc/crd is non-linear: "translate" the raster-location of the controlpoints, instead of translating the coordinates
            QSharedPointer<PlanarCTPGeoReference> ctpgrf = grf->as<PlanarCTPGeoReference>();
            quint32 nrOfControlPoints = ctpgrf->nrControlPoints();
            for (quint32 i = 0; i < nrOfControlPoints; ++i) {
                Pixeld location = ctpgrf->controlPoint(i).gridLocation();
                location -= pixStart;
                ctpgrf->controlPoint(i).gridLocation(location);
            }
        }
        grf->size(Size<>(columns, lines, 1)); // the "loadGeoref" call has "overwritten" these values with the one of the parent-georef
        grf->compute();
        return true;
    } else if ( type == "GeoRefCTP"){
        return loadGeorefTiepoints(odf, grf);
    }
    return false;
}

bool GeorefConnector::loadGeorefTiepoints(const IniFile& odf, GeoReference *grf) {
    QString csyName = odf.value("GeoRef","CoordSystem");
    QUrl path = mastercatalog()->name2url(csyName, itCOORDSYSTEM);
    ICoordinateSystem csy;
    if(!csy.prepare(path.toString())) {
        return ERROR2(ERR_COULD_NOT_LOAD_2, "coordinate system",csyName);
    }
    grf->coordinateSystem(csy);
    QSharedPointer<PlanarCTPGeoReference> grfctp(grf->as<PlanarCTPGeoReference>());
    BinaryIlwis3Table tbl;
    tbl.load(_odf);
    std::vector<int> colindexes(10, iUNDEF);
    enum ColIndexID{ciiX=0, ciiY=1, ciiZ=2, ciiLAT=3, ciiLON=4, ciiXP=5, ciiYP=6, ciiACT=7, ciiDX=8, ciiDY=9, ciiNEVER=10};
    colindexes[ciiX] = tbl.index("X");
    colindexes[ciiY] = tbl.index("Y");
    colindexes[ciiZ] = tbl.index("Z");
    colindexes[ciiXP] = tbl.index("Col");
    colindexes[ciiYP] = tbl.index("Row");
    colindexes[ciiACT] = tbl.index("Active");
    //colindexes[ciiDX] = tbl.index("DCol");
    //colindexes[ciiDY] = tbl.index("DRow");
    for(int rec = 0; rec < tbl.rows(); ++rec){
        double x=rUNDEF;
        tbl.get(rec,colindexes[ciiX],x);
        double y=rUNDEF;
        tbl.get(rec,colindexes[ciiY],y);
            double z=0;
        if (colindexes[ciiZ] != iUNDEF) {
            tbl.get(rec,colindexes[ciiZ],z);
        }
        double xp=rUNDEF;
        tbl.get(rec,colindexes[ciiXP],xp);
        double yp=rUNDEF;
        tbl.get(rec,colindexes[ciiYP],yp);

        double active=1;
        tbl.get(rec,colindexes[ciiACT],active);
        ControlPoint ctp(Coordinate(x,y,z),Pixeld(xp,yp));
        ctp.active(active != 0);
        grfctp->setControlPoint(ctp);
    }
    QString transf = odf.value("GeoRefCTP", "Transformation");
    PlanarCTPGeoReference::Transformation transformation;
    if (transf == "Conform")
      transformation =  PlanarCTPGeoReference::tCONFORM;
    else if (transf == "Affine")
      transformation =  PlanarCTPGeoReference::tAFFINE;
    else if (transf == "SecondOrder")
      transformation =  PlanarCTPGeoReference::tSECONDORDER;
    else if (transf == "FullSecondOrder")
      transformation =  PlanarCTPGeoReference::tFULLSECONDORDER;
    else if (transf == "ThirdOrder")
      transformation =  PlanarCTPGeoReference::tTHIRDORDER;
    else if (transf == "Projective")
      transformation =  PlanarCTPGeoReference::tPROJECTIVE;
    grfctp->transformation(transformation);
    grfctp->compute();
	
	QString slave = odf.value("GeoRefCTP", "Reference Map");
	if (slave != sUNDEF) {
		slave = grf->resource().container(true).toString() + "/" + slave;
		grf->resourceRef().addProperty("slaveraster", slave);
	}
    return true;
}

bool GeorefConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    Ilwis3Connector::loadMetaData(data, options);
    IniFile *odf = _odf.data();

    return loadGeoref(*odf, data);
}

bool GeorefConnector::storeMetaData(IlwisObject *obj, const IOOptions &options)
{
    Ilwis3Connector::storeMetaData(obj, itGEOREF);
    GeoReference *grf = static_cast<GeoReference *>(obj);
    QString localPathCsy;

    const ICoordinateSystem csy = grf->coordinateSystem();
    if (csy.isValid()) {
        // write the corresponding coordinate system
        localPathCsy = writeCsy(obj, csy);
    } else
        localPathCsy = "unknown.csy";

    _odf->setKeyValue("GeoRef","CoordSystem", localPathCsy);
    Size<> sz = grf->size();
    _odf->setKeyValue("GeoRef","Lines", IniFile::FormatElement(sz.ysize()));
    _odf->setKeyValue("GeoRef","Columns", IniFile::FormatElement(sz.xsize()));
    //CornersGeoReference *cgrf = dynamic_cast<CornersGeoReference *>(grf);

    if ( grf->grfType<CornersGeoReference>()) {
        QString georefFilename(sourceRef().toLocalFile());
        QFileInfo georefFileInfo(georefFilename);
        IGeoReference existingGeoref(sourceRef());
        if (georefFileInfo.exists() ) {
            if (!grf->isCompatible(existingGeoref.ptr())) {
                georefFilename = OSHelper::ensureUniqueFilename(georefFilename);
            } else
                return true; // no need to rewrite the georef file with the same information
        }
        QSharedPointer<CornersGeoReference> cgrf = grf->as<CornersGeoReference>();
        _odf->setKeyValue("GeoRef","Type", "GeoRefCorners");
        Coordinate cmin = cgrf->envelope().min_corner();
        Coordinate cmax = cgrf->envelope().max_corner();
        _odf->setKeyValue("GeoRefCorners","CornersOfCorners", cgrf->centerOfPixel() ? "No" : "Yes");
        _odf->setKeyValue("GeoRefCorners", "MinX", IniFile::FormatElement(cmin.x));
        _odf->setKeyValue("GeoRefCorners", "MinY", IniFile::FormatElement(cmin.y));
        _odf->setKeyValue("GeoRefCorners", "MaxX", IniFile::FormatElement(cmax.x));
        _odf->setKeyValue("GeoRefCorners", "MaxY", IniFile::FormatElement(cmax.y));

        std::vector<double> matrix = cgrf->matrix();
        _odf->setKeyValue("GeoRefSmpl", "a11", IniFile::FormatElement(matrix[0]));
        _odf->setKeyValue("GeoRefSmpl", "a22", IniFile::FormatElement(matrix[3]));
        _odf->setKeyValue("GeoRefSmpl", "a12", "0");
        _odf->setKeyValue("GeoRefSmpl", "a21", "0");

         std::vector<double> support = cgrf->support();
         _odf->setKeyValue("GeoRefSmpl", "b1", IniFile::FormatElement(support[0]));
         _odf->setKeyValue("GeoRefSmpl", "b2", IniFile::FormatElement(support[1]));

         _odf->store("grf", georefFilename);

        return true;
    }

    return false;

}

bool GeorefConnector::loadGeorefCorners(const IniFile& odf, IlwisObject *data) {
    GeoReference *grf = static_cast<GeoReference *>(data);
    QString csyName = odf.value("GeoRef","CoordSystem");
    ICoordinateSystem csy;
    if ( csyName.toLower() == "latlonwgs84.csy"){
        Resource resource = mastercatalog()->name2Resource("code=epsg:4326", itCOORDSYSTEM);
        if(!csy.prepare(resource))
            return false;
    }else {
        if ( csyName == "unknown.csy" || csyName == "" || csyName == sUNDEF){
            Resource resource = mastercatalog()->name2Resource("code=csy:unknown", itCOORDSYSTEM);
            if(!csy.prepare(resource)) {
                return false;
            }
        }else {
            QUrl path;
            if ( csyName.indexOf(":/") == -1){
                int index = odf.url().lastIndexOf("/");
                path = odf.url().left(index) + "/" + csyName;
            }else
                path = QUrl::fromLocalFile(csyName).toString();
            if( !csy.prepare(path.toString())) {
                kernel()->issues()->log(TR("Couldn't find coordinate system %1, defaulting to unknown").arg(csyName),IssueObject::itWarning);
                Resource resource = mastercatalog()->name2Resource("code=csy:unknown", itCOORDSYSTEM);
                if(!csy.prepare(resource)) {
                    return false;
                }
            }
        }
    }

    grf->coordinateSystem(csy);
    double maxx = odf.value("GeoRefCorners","MaxX").toDouble();
    double maxy = odf.value("GeoRefCorners","MaxY").toDouble();
    double minx = odf.value("GeoRefCorners","MinX").toDouble();
    double miny = odf.value("GeoRefCorners","MinY").toDouble();
    if ( maxx == rUNDEF || maxy == rUNDEF || minx == rUNDEF || miny == rUNDEF) {
        kernel()->issues()->log(TR("Uninitialized boundaries for georeference %1").arg(_resource.name()));
        return false;
    }
	bool centerOfCornerPixels = (odf.value("GeoRefCorners", "CornersOfCorners").compare("No") == 0);
	QSharedPointer< CornersGeoReference> spGrf = grf->as< CornersGeoReference>();
	grf->coordinateSystem()->envelope(Envelope(Coordinate(minx, miny), Coordinate(maxx, maxy)));
	spGrf->internalEnvelope(Envelope(Coordinate(minx, miny), Coordinate(maxx, maxy)));
    grf->centerOfPixel(centerOfCornerPixels);
    grf->compute();
    return true;
}


void GeorefConnector::createGeoreference(const IniFile &odf, GeoReference *grf) const{
    //todo georefFACTOR
    QString type = odf.value("GeoRef","Type");
    if ( type == "GeoRefCorners"){
        grf->create("corners");
    }
    else if ( type == "GeoRefCTP") {
        grf->create("tiepoints");
    }
    else if ( type == "GeoRefNone") {
        grf->create("undetermined");
    }
    else if ( type == "GeoRefSubMap") {
        auto name = odf.value("GeoRefSubMap","GeoRef");
        QUrl resource = mastercatalog()->name2url(name, itGEOREF);
        IniFile odf;
        odf.setIniFile(resource.toLocalFile());
        createGeoreference(odf, grf);
    }
}

IlwisObject *GeorefConnector::create() const
{
    GeoReference * grf = new GeoReference(_resource);
    IniFile *odf = _odf.data();
    createGeoreference(*odf, grf);
    return grf;
}

QString GeorefConnector::format() const
{
    return "georef";
}
