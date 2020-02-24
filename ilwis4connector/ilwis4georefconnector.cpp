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
#include "georeference.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "controlpoint.h"
#include "ctpgeoreference.h"
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
	QJsonObject jroot;

	store(obj, options, jroot);

	flush(obj, jroot);

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
			jctp.insert("latlonlocation", ctpgrf->controlPoint(i).llLocation().toString());
			jctp.insert("projlocation", ctpgrf->controlPoint(i).toString());
			jctp.insert("gridlocation", ctpgrf->controlPoint(i).gridLocation().toString());
			jctp.insert("isactive", ctpgrf->controlPoint(i).isActive());
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
	if (!Ilwis4Connector::loadMetaData(obj, options))
		return false;


	return true;
}

bool Ilwis4GeorefConnector::loadData(IlwisObject* obj, const IOOptions& options) {

	return true;
}
