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
#include "boundsonlycoordinatesystem.h"
#include "projection.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "ilwis4connector.h"
#include "Ilwis4coordinatesystemconnector.h"
#include "ilwisobjectconnector.h"
#include "factory.h"
#include "abstractfactory.h"
#include "projectionfactory.h"


using namespace Ilwis;
using namespace Ilwis4C;

ConnectorInterface *Ilwis4CoordinateSystemConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions& options)
{
	return new Ilwis4CoordinateSystemConnector(resource, load, options);
}

Ilwis4CoordinateSystemConnector::Ilwis4CoordinateSystemConnector(const Ilwis::Resource &resource, bool load, const IOOptions& options) : Ilwis4Connector(resource, load, options)
{
	_version = 1;
}

bool Ilwis4CoordinateSystemConnector::store(IlwisObject *obj, const IOOptions &options)
{
	QJsonArray objects;
	QJsonObject jroot, jcsy;

	IOOptions newOptions = options;
	newOptions.addOption("version", _version);

	store(obj, newOptions, jcsy);
	jroot.insert("ilwisobject", jcsy);
	objects.append(jroot);

	flush(obj, objects);

	storeData(obj, options);

	return true;
}

ICoordinateSystem Ilwis4CoordinateSystemConnector::createCsy(const IOOptions& options, const QJsonObject& jcsy) {
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
	return csy;
}
bool Ilwis4CoordinateSystemConnector::loadMetaData(IlwisObject* object, const IOOptions& options, const QJsonValue& jvalue) {
	

	Ilwis4Connector::loadMetaData(object, options, jvalue);

	CoordinateSystem *csy = static_cast<CoordinateSystem *>(object);

	if (csy->ilwisType() == itCONVENTIONALCOORDSYSTEM) {
		ConventionalCoordinateSystem *ccsy = static_cast<ConventionalCoordinateSystem *>(csy);
		const ProjectionFactory *factory = kernel()->factory<ProjectionFactory>("ProjectionFactory", "proj4");
		if (!factory)
			return false;
		IProjection proj;
		proj.prepare();
		QString proj4Def = jvalue["projection"].toString();
		
		QString projtype = jvalue["projectioncode"].toString();
		ProjectionImplementation *impl = factory->create(projtype, proj4Def);
		if (!impl)
			return false;
		proj->setImplementation(impl);
		ccsy->setProjection(proj);

		QString sell = jvalue["ellipsoid"].toString();
		QStringList parts = sell.split(" ");
		if (parts.size() == 2) {
			double a = parts[0].toDouble();
			double f = parts[1].toDouble();
			IEllipsoid ell;
			ell.prepare();
			ell->setEllipsoid(a, f);
			ccsy->setEllipsoid(ell);
		}
		else {
			IEllipsoid ell;
			ell.prepare(sell);
			ccsy->setEllipsoid(ell);
		}
		QJsonValue jdatum = jvalue["datum"];
		if (jdatum != QJsonValue::Undefined) {
			QJsonArray parms = jdatum["parameters"].toArray();
			std::vector<double> dparms;
			for (auto& parm : parms) {
				dparms.push_back(parm.toDouble());
			}
			GeodeticDatum *gdatum = new GeodeticDatum(dparms, ccsy->ellipsoid());
			gdatum->name(jdatum["name"].toString());
			gdatum->setArea(jdatum["area"].toString());
			gdatum->setDescription(jdatum["description"].toString());
			gdatum->setAuthority(jdatum["authority"].toString());
			gdatum->code(jdatum["code"].toString());
			ccsy->setDatum(gdatum);
		}

	}
	QString env = jvalue["envelope"].toString();
	csy->envelope(env);

	return true;


}
bool Ilwis4CoordinateSystemConnector::store(IlwisObject *obj, const IOOptions& options, QJsonObject& jcsy) {
	CoordinateSystem *csy = static_cast<CoordinateSystem *>(obj);
	Ilwis4Connector::store(obj, options, jcsy);

	if (csy->ilwisType() == itCONVENTIONALCOORDSYSTEM) {
		ConventionalCoordinateSystem *ccsy = static_cast<ConventionalCoordinateSystem *>(csy);
		jcsy.insert("projectioncode", ccsy->projection()->code());
		jcsy.insert("projection", ccsy->projection()->toProj4());
		if (ccsy->ellipsoid()->code() != "?")
			jcsy.insert("ellipsoid", ccsy->ellipsoid()->code());
		else
			jcsy.insert("ellipsoid", QString("%1 %2").arg(ccsy->ellipsoid()->majorAxis()).arg(ccsy->ellipsoid()->flattening()));

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
			jcsy.insert("datum", jdatum);
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
	QFileInfo inf = _resource.url(true).toLocalFile();
	QFile file;
	file.setFileName(inf.absoluteFilePath());
	if (file.open(QIODevice::ReadOnly)) {
		QString meta = file.readAll();
		QJsonDocument doc = QJsonDocument::fromJson(meta.toUtf8());
		if (!doc.isNull()) {
			QJsonArray jobjects = doc.array();
			QJsonValue jvalue = jobjects.at(0);
			return loadMetaData(obj, options, jvalue["ilwisobject"]);
		}
	}
	return true;
}

bool Ilwis4CoordinateSystemConnector::loadData(IlwisObject* obj, const IOOptions& options) {

	return true;
}
