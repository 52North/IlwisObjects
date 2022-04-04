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
#include <QDataStream>
#include "version.h"
#include "ilwisdata.h"
#include "coverage.h"
#include "connectorinterface.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "coordinatesystem.h"

#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "ilwisobjectconnector.h"
#include "ilwis4connector.h"
#include "ilwis4featureconnector.h"
#include "ilwisobjectconnector.h"
#include "ilwis4tableconnector.h"
#include "ilwis4domainconnector.h"
#include "ilwis4coordinatesystemconnector.h"
#include "factory.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorfactory.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#include "quazip//quazipdir.h"


using namespace Ilwis;
using namespace Ilwis4C;

ConnectorInterface *Ilwis4FeatureConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions& options)
{
	return new Ilwis4FeatureConnector(resource, load, options);
}

Ilwis4FeatureConnector::Ilwis4FeatureConnector(const Ilwis::Resource &resource, bool load, const IOOptions& options) : Ilwis4Connector(resource, load, options)
{
	_version = 1;
}

bool Ilwis4FeatureConnector::store(IlwisObject *obj, const IOOptions &options)
{
	QJsonArray objects;
	QJsonObject jroot, jfeatures;

	IOOptions newOptions = options;
	newOptions.addOption("version", _version);

	store(obj, newOptions, jfeatures);

	jroot.insert("ilwisobject", jfeatures);
	objects.append(jroot);

	flush(obj, objects);

	storeData(obj, options);

	return true;
}
bool Ilwis4FeatureConnector::store(IlwisObject *obj, const IOOptions& options, QJsonObject& jfeatures) {
	

	FeatureCoverage *features = static_cast<FeatureCoverage *>(obj);
	Ilwis4Connector::store(obj, options, jfeatures);

	auto opt = options;
	opt["status"] = "support";

	QJsonObject jdata, jstackdomain;
	if (features->attributeDefinitions().domain().isValid()) {
		Ilwis4DomainConnector::store(features->attributeDefinitions().domain().ptr(), opt, jstackdomain);
		std::vector<QString> indexes = features->attributeDefinitions().indexes();

		QString sindexes;
		for (auto index : indexes) {
			if (sindexes != "")
				sindexes += "|";
			sindexes += index;
		}
		jstackdomain.insert("stackindexes", sindexes);
		jdata.insert("stackdomain", jstackdomain);
	}
	QJsonObject jattributes;
	Ilwis4TableConnector::store(features->attributeTable().ptr(), opt, jattributes);
	jdata.insert("attributes", jattributes);

	jfeatures.insert("data", jdata);


	qint32 fcount = features->featureCount(itPOINT);
	jfeatures.insert("pointcount", fcount);
	fcount = features->featureCount(itLINE);
	jfeatures.insert("linecount", fcount);
	fcount = features->featureCount(itPOLYGON);
	jfeatures.insert("polygoncount", fcount);

	QJsonObject jcsy;
	Ilwis4CoordinateSystemConnector::store(features->coordinateSystem().ptr(), opt, jcsy);

	jfeatures.insert("coordinatesystem", jcsy);
	jfeatures.insert("envelope", features->envelope().toString());

	QString path = obj->resource().url(true).toLocalFile();
	QFileInfo inf(path);
	jfeatures.insert("binarydata", inf.baseName() + ".zip");


	return true;
}

bool Ilwis4FeatureConnector::storeData(IlwisObject *obj, const IOOptions &options) {


	const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
	if (!factory) {
		kernel()->issues()->log("Couldn't find factory for gdal connector");
		return false;
	}
	Resource res = obj->resource();
	QString path = res.url(true).toString();
	path.replace(".ilwis4", ".geojson");
	res.setUrl(path, true, false);
	res.setUrl(path, false, false);
	ConnectorInterface *connector = factory->createFromResource<>(res, "gdal");
	connector->format("GeoJSON");
    connector->store(obj, options);

	delete connector;

	QString localPath = res.url(true).toLocalFile();
	QFile file(localPath);
	if (file.open(QIODevice::ReadOnly)) {
		QString zipPath = localPath;
		zipPath.replace(".geojson", ".zip");
		QuaZip zipfile(zipPath);
		zipfile.open(QuaZip::mdCreate);
		QuaZipFile filez(&zipfile);
		filez.open(QIODevice::WriteOnly, QuaZipNewInfo(res.name()));

		QTextStream ts(&file);
		filez.write(ts.readAll().toUtf8());

		filez.close();
		zipfile.close();
		file.remove();

	}

	return true;
}

bool Ilwis4FeatureConnector::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
	QFileInfo inf = _resource.url(true).toLocalFile();
	QFile file;
	file.setFileName(inf.absoluteFilePath());
	if (file.open(QIODevice::ReadOnly)) {
		QString meta = file.readAll();
		QJsonDocument doc = QJsonDocument::fromJson(meta.toUtf8());
		if (!doc.isNull()) {
			FeatureCoverage *features = static_cast<FeatureCoverage *>(obj);
			QJsonArray jobjects = doc.array();
			QJsonValue jvalue = jobjects.at(0);
			QJsonValue jfeatures = jvalue["ilwisobject"];
			Ilwis4Connector::loadMetaData(obj, options, jfeatures);
			_datafile = jfeatures["binarydata"].toString();

			QJsonObject jdata = jfeatures["data"].toObject();
			QJsonObject jstackd = jdata["stackdomain"].toObject();
			if (!jstackd.isEmpty()) {
				IDomain dom = Ilwis4DomainConnector::createDomain(options, jstackd);
				Ilwis4DomainConnector::loadMetaData(dom.ptr(), options, jstackd);

				QStringList indexes = jstackd["stackindexes"].toString().split("|");
				std::vector<QString> variants(indexes.size());
				for (int i = 0; i < indexes.size(); ++i) {
					variants[i] = indexes[i];
				}
				features->attributeDefinitionsRef().setSubDefinition(dom, variants);
			}
			int fcount = jfeatures["pointcount"].toInt();
			features->setFeatureCount(itPOINT, fcount,0);
			fcount = jfeatures["linecount"].toInt();
			features->setFeatureCount(itLINE, fcount, 0);
			fcount = jfeatures["polygoncount"].toInt();
			features->setFeatureCount(itPOLYGON, fcount, 0);

			QJsonValue jattributes = jdata["attributes"];
			QJsonArray jdefs = jattributes["columndefinitions"].toArray();
			int idx = 0;
			for (auto jcdefr : jdefs) {
				QJsonObject jcdef = jcdefr.toObject();
				QString cname = jcdef["name"].toString();
				QJsonObject jdef = jcdef["datadefinition"].toObject();
				DataDefinition def;
				Ilwis4Connector::loadDataDef(def, jdef);
				features->attributeDefinitionsRef().addColumn(ColumnDefinition(cname,  def, idx++));
			}

			QJsonValue jcsy = jfeatures["coordinatesystem"];
			ICoordinateSystem csy = Ilwis4CoordinateSystemConnector::createCsy(options, jcsy.toObject());
			Ilwis4CoordinateSystemConnector::loadMetaData(csy.ptr(), options, jcsy);
			features->coordinateSystem(csy);
			QString env = jfeatures["envelope"].toString();
			features->envelope(env);
		}
	}

	return true;
}

bool Ilwis4FeatureConnector::loadData(IlwisObject* obj, const IOOptions& options) {

	if (!dataIsLoaded()) {
		const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
		if (!factory) {
			kernel()->issues()->log("Couldn't find factory for gdal connector");
			return false;
		}
		Resource res = obj->resource();
		QString path = res.url(true).toString();
		path.replace(".ilwis4", ".geojson");
		res.setUrl(path, true, false);
		res.setUrl(path, false, false);
		QFileInfo inf(res.url(true).toLocalFile());
		QString prefix = "/vsizip/" + inf.path() + "/" + _datafile + "/";

		ConnectorInterface *connector = factory->createFromResource<>(res, "gdal", { "prefix", prefix });
		connector->format("GeoJSON");
		connector->loadData(obj, options);

		_binaryIsLoaded = connector->dataIsLoaded();
		delete connector;
	}

	return true;

}
