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
#include "geometries.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "coordinatesystem.h"
#include "conventionalcoordinatesystem.h"
#include "projection.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "ilwis4connector.h"
#include "Ilwis4coordinatesystemconnector.h"
#include "ilwisobjectconnector.h"
#include "factory.h"
#include "abstractfactory.h"


using namespace Ilwis;
using namespace Ilwis4C;

ConnectorInterface *Ilwis4CoordinateSystemConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions& options)
{
	return new Ilwis4CoordinateSystemConnector(resource, load, options);
}

Ilwis4CoordinateSystemConnector::Ilwis4CoordinateSystemConnector(const Ilwis::Resource &resource, bool load, const IOOptions& options) : Ilwis4Connector(resource, load, options)
{
}

bool Ilwis4CoordinateSystemConnector::store(IlwisObject *obj, const IOOptions &options)
{
	QJsonObject jroot;

	store(obj, options, jroot);

	flush(obj, jroot);

	return true;
}
bool Ilwis4CoordinateSystemConnector::store(IlwisObject *obj, const IOOptions& options, QJsonObject& jcsy) {
	CoordinateSystem *csy = static_cast<CoordinateSystem *>(obj);
	Ilwis4Connector::store(obj, options, jcsy);

	if (csy->ilwisType() == itCONVENTIONALCOORDSYSTEM) {
		ConventionalCoordinateSystem *ccsy = static_cast<ConventionalCoordinateSystem *>(csy);
		jcsy.insert("projection", ccsy->projection()->toProj4());
		jcsy.insert("ellipsoid", ccsy->ellipsoid()->toProj4());

		const std::unique_ptr<GeodeticDatum>& datum = ccsy->datum();
		if (datum) {
			QJsonObject jdatum;
			jdatum.insert("name", datum->name());
			jdatum.insert("code", datum->code());
			jdatum.insert("description", datum->description());
			jdatum.insert("area", datum->area());
			jdatum.insert("authority", datum->authority());
			QJsonArray parms;
			for (int i = 0; i < 10; ++i) {
				parms.append(datum->parameter((GeodeticDatum::DatumParameters)i));
			}
			jdatum.insert("parameters", parms);
		}
		jcsy.insert("unit", ccsy->unit());
	}
	jcsy.insert("envelope", csy->envelope().toString());

	return true;
}

bool Ilwis4CoordinateSystemConnector::storeData(IlwisObject *obj, const IOOptions &options) {

	return true;
}

bool Ilwis4CoordinateSystemConnector::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
	if (!Ilwis4Connector::loadMetaData(obj, options))
		return false;


	return true;
}

bool Ilwis4CoordinateSystemConnector::loadData(IlwisObject* obj, const IOOptions& options) {

	return true;
}
