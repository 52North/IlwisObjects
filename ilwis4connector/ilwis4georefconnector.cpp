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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "version.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "ilwis4connector.h"
#include "ilwisobjectconnector.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "conventionalcoordinatesystem.h"
#include "georeference.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "controlpoint.h"
#include "ctpgeoreference.h"
#include "boundsonlycoordinatesystem.h"
#include "eigen3/Eigen/LU"
#include "mathhelper.h"
#include "planarctpgeoreference.h"
#include "undeterminedgeoreference.h"
#include "ilwis4coordinatesystemconnector.h"
#include "ilwis4georefconnector.h"


using namespace Ilwis;
using namespace Ilwis4C;

ConnectorInterface *Ilwis4GeorefConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions& options)
{
	return new Ilwis4GeorefConnector(resource, load, options);
}

Ilwis4GeorefConnector::Ilwis4GeorefConnector(const Ilwis::Resource &resource, bool load, const IOOptions& options) : Ilwis4Connector(resource, load, options)
{
}

bool Ilwis4GeorefConnector::store(IlwisObject *obj, const IOOptions &options)
{
	QJsonArray objects;
	QJsonObject jroot;

	store(obj, options, jroot);
	objects.append(jroot);

	flush(obj, objects);

	storeData(obj, options);

	return true;
}

bool Ilwis4GeorefConnector::loadMetaData(IlwisObject* object, const IOOptions& options, const QJsonValue& jvalue) {
	Ilwis4Connector::loadMetaData(object, options, jvalue);

	GeoReference *grf = static_cast<GeoReference *>(object);


	QString typen = jvalue["typename"].toString();
	
	if (typen == CTPGeoReference::typeName()) {
		grf->create(CTPGeoReference::typeName());
		QJsonArray jtpoints = jvalue["controlpoints"].toArray();
		QSharedPointer<PlanarCTPGeoReference> ctpgrf = grf->as<PlanarCTPGeoReference>();
		for (auto jtpoint : jtpoints) {
			QJsonObject tp = jtpoint.toObject();
			QString ll = tp["latlonlocation"].toString();
			QStringList parts = ll.split(",");
			LatLon llLocation({ parts[0], parts[1] });
			Coordinate crdPrj(tp["projlocation"].toString());
			Pixeld pix(tp["gridlocation"].toString());
			ControlPoint ctp( crdPrj, llLocation,  pix);
			bool active = tp["isactive"].toBool();
			ctp.active(active);
			ctpgrf->setControlPoint(ctp);
		}
		int transformation = jvalue["transformation"].toInt();
		ctpgrf->transformation((PlanarCTPGeoReference::Transformation)transformation);
		QString slave = jvalue["slaveraster"].toString();
		grf->resourceRef()["slaveraster"] = slave;

	}
	else if (typen == CornersGeoReference::typeName()) {
		grf->create(CornersGeoReference::typeName());
		QSharedPointer< CornersGeoReference> spGrf = grf->as< CornersGeoReference>();
		spGrf->internalEnvelope(jvalue["envelope"].toString());
		spGrf->compute();
	}
	else if (typen == UndeterminedGeoReference::typeName()) {

	}
	QJsonValue jsize = jvalue["size"];
	grf->size(jsize.toString());
	grf->centerOfPixel(jvalue["centerofpixel"].toBool());

	QJsonValue jcsy = jvalue["coordinatesystem"];
	QJsonValue jcsybase = jcsy["base"];
	IlwisTypes tp = TypeHelper::name2type(jcsybase["ilwistype"].toString());
	ICoordinateSystem csy;
	if (tp == itCONVENTIONALCOORDSYSTEM) {
		IConventionalCoordinateSystem ccsy;
		ccsy.prepare();
		csy = ccsy;
	}
	if (tp == itBOUNDSONLYCSY) {
		IBoundsOnlyCoordinateSystem bcsy;;
		bcsy.prepare();
		csy = bcsy;
	}
	Ilwis4CoordinateSystemConnector::loadMetaData(csy.ptr(), options, jcsy);
	grf->coordinateSystem(csy);

	return true;

}
bool Ilwis4GeorefConnector::store(IlwisObject *obj, const IOOptions& options, QJsonObject& jgrf) {

	Ilwis4Connector::store(obj, options, jgrf);
	GeoReference *grf = static_cast<GeoReference *>(obj);
	jgrf.insert("size", grf->size().toString());
	jgrf.insert("centerofpixel", grf->centerOfPixel());
	QJsonObject jcsy;
	Ilwis4CoordinateSystemConnector::store(grf->coordinateSystem().ptr(), options, jcsy);
	jgrf.insert("coordinatesystem", jcsy);

	if (grf->grfType<CornersGeoReference>()) {
		jgrf.insert("typename", CornersGeoReference::typeName());
		jgrf.insert("envelope", grf->envelope().toString());
	}
	else if (grf->grfType<CTPGeoReference>()) {
		jgrf.insert("typename", CTPGeoReference::typeName());
		QSharedPointer<PlanarCTPGeoReference> ctpgrf = grf->as<PlanarCTPGeoReference>();
		QJsonArray jctps;
		int nrOfControlPoints = ctpgrf->nrControlPoints();
		for (int i = 0; i < nrOfControlPoints; ++i) {
			QJsonObject jctp;
			ControlPoint ctp = ctpgrf->controlPoint(i);
			jctp.insert("latlonlocation", ctp.llLocation().toString());
			jctp.insert("projlocation", ctp.toString());
			jctp.insert("gridlocation", ctp.gridLocation().toString());
			jctp.insert("isactive", ctp.isActive());
			jctps.append(jctp);
		}
		jgrf.insert("controlpoints", jctps);
		jgrf.insert("transformation", (int)ctpgrf->transformation());
		jgrf.insert("slaveraster", grf->resourceRef()["slaveraster"].toString());
	}
	else if (grf->grfType<UndeterminedGeoReference>()) {
		jgrf.insert("typename", UndeterminedGeoReference::typeName());
	}
	return true;
}

bool Ilwis4GeorefConnector::storeData(IlwisObject *obj, const IOOptions &options) {

	return true;
}

bool Ilwis4GeorefConnector::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
	return true;
}

bool Ilwis4GeorefConnector::loadData(IlwisObject* obj, const IOOptions& options) {

	return true;
}
