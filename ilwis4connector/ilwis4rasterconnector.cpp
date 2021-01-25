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
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "factory.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorfactory.h"
#include "table.h"
#include "raster.h"
#include "ilwisobjectconnector.h"
#include "ilwis4connector.h"
#include "ilwis4rasterconnector.h"
#include "ilwis4coordinatesystemconnector.h"
#include "ilwis4georefconnector.h"
#include "ilwis4domainconnector.h"
#include "ilwis4tableconnector.h"
#include "ilwisobjectconnector.h"



using namespace Ilwis;
using namespace Ilwis4C;

ConnectorInterface *Ilwis4RasterConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions& options)
{
	return new Ilwis4RasterConnector(resource, load, options);
}

Ilwis4RasterConnector::Ilwis4RasterConnector(const Ilwis::Resource &resource, bool load, const IOOptions& options) : Ilwis4Connector(resource, load, options)
{
	_version = 1;
}

bool Ilwis4RasterConnector::store(IlwisObject *obj, const IOOptions &options)
{
	QJsonArray objects;
	QJsonObject jroot, jraster;
	IOOptions newOptions = options;
	newOptions.addOption("version", _version);
	store(obj, newOptions, jraster);

	jroot.insert("ilwisobject", jraster);
	objects.append(jroot);

	flush(obj, objects);

	storeData(obj, options);

	return true;
}
bool Ilwis4RasterConnector::store(IlwisObject *obj, const IOOptions& options, QJsonObject& jraster) {

	auto opt = options;
	opt["status"] = "support";
	RasterCoverage *raster = static_cast<RasterCoverage *>(obj);
	Ilwis4Connector::store(obj, options, jraster);
	QJsonObject jgrf;
	Ilwis4GeorefConnector::store(raster->georeference().ptr(), opt, jgrf);

	jraster.insert("georeference", jgrf);

	QJsonObject jdata;
	storeDataDef(raster->datadef(), jdata);

	QJsonArray jbandDefs;
	for (int index = 0; index < raster->size().zsize(); ++index) {
		const DataDefinition& def = raster->datadef(index);
		QJsonObject jdatadef;
		storeDataDef(def, jdatadef);
		jbandDefs.append(jdatadef);
	}
	jdata.insert("banddefinitions", jbandDefs);

	QJsonObject jstackDomain;
	Ilwis4DomainConnector::store(raster->stackDefinition().domain().ptr(), opt, jstackDomain);
	std::vector<QString> indexes = raster->stackDefinition().indexes();
	QString sindexes;
	for (auto index : indexes) {
		if (sindexes != "")
			sindexes += "|";
		sindexes += index;
	}
	jstackDomain.insert("stackindexes", sindexes);
	jdata.insert("stackdomain", jstackDomain);
	jraster.insert("data", jdata);

	QJsonObject jattributes;
	if (raster->hasAttributes() && raster->attributeTable()->recordCount() > 0) {
		raster->attributeTable()->store();
		jraster.insert("attributes", raster->attributeTable()->resourceRef().url(true).toString());
		jraster.insert("primarykey", raster->primaryKey());
	}
	if (raster->histogramCalculated() && raster->datadef().domain()->ilwisType() == itNUMERICDOMAIN) {
		auto bins = raster->datadef().statistics().histogram();
		if (raster->datadef().statistics().histogramMode() == ContainerStatistics<PIXVALUETYPE>::pHISTOGRAM){ // quick histograms arent saved
			QString histogram;
			for (auto& bin : bins) {
				if (histogram != "") histogram += " ";
				histogram += QString::number(bin._limit) + "|" + QString::number(bin._count);
			}
			jraster.insert("histogram", histogram);
		}
	}
	Resource res = obj->resource(IlwisObject::cmOUTPUT);
	QString path = res.url(true).toLocalFile();
	QFileInfo inf(path);
	jraster.insert("binarydata", inf.baseName() + ".tif_");
	jraster.insert("size", raster->size().toString());
	
	return true;
}

bool Ilwis4RasterConnector::storeData(IlwisObject *obj, const IOOptions &options) {

	const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
	if (!factory) {
		kernel()->issues()->log("Couldn't find factory for gdal connector");
		return false;
	}

	ConnectorInterface *connector = factory->createFromResource<>(obj->resource(), "gdal");
	connector->format("GTiff");

	Resource resOut = obj->resource(IlwisObject::cmOUTPUT);
	QString pathOut = resOut.url(true).toLocalFile().remove(".ilwis4");
	pathOut += ".tif_";

	connector->store(obj,{"outputname", pathOut});

	delete connector;
	return true;
}

bool Ilwis4RasterConnector::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
	QFileInfo inf = _resource.url(true).toLocalFile();
	QFile file;
	file.setFileName(inf.absoluteFilePath());
	if (file.open(QIODevice::ReadOnly)) {
		QString meta = file.readAll();
		QJsonDocument doc = QJsonDocument::fromJson(meta.toUtf8());
		if (!doc.isNull()) {
			RasterCoverage *raster = static_cast<RasterCoverage *>(obj);
			QJsonArray jobjects = doc.array();
			QJsonValue jvalue = jobjects.at(0);
			QJsonValue jraster = jvalue["ilwisobject"];
			bool issubRaster = obj->resource().code().indexOf("band=") == 0;
			if (issubRaster) {
				int band = obj->resource().code().mid(5).toInt();
				_datafile = jraster["binarydata"].toString();
				IGeoReference grf;
				grf.prepare();
				Ilwis4GeorefConnector::loadMetaData(grf.ptr(), options, jraster["georeference"].toObject());

				raster->georeference(grf);
				raster->size(jraster["size"].toString());
				QJsonObject jdata = jraster["data"].toObject();
				QJsonObject jstackDomain = jdata["stackdomain"].toObject();
				IDomain ilobj = Ilwis4DomainConnector::createDomain(options, jstackDomain);
				Ilwis4DomainConnector::loadMetaData(ilobj.ptr(), options, jstackDomain);
				QStringList indexes = jstackDomain["stackindexes"].toString().split("|");
				std::vector<QString> variants(1);

				QJsonArray jbanddefs = jdata["banddefinitions"].toArray();
				for (int i = 0; i < raster->size().zsize(); ++i) {
					QJsonObject jbanddef = jbanddefs[i].toObject();
					if (i == band) {
						Ilwis4Connector::loadDataDef(raster->datadefRef(i), jbanddef);
						raster->datadefRef() = raster->datadefRef(i);
						variants[0] = indexes[i];
						raster->stackDefinitionRef().setSubDefinition(ilobj, variants);
						break;
					}
				}
			}
			else {
				Ilwis4Connector::loadMetaData(obj, options, jraster);
				_datafile = jraster["binarydata"].toString();

				IGeoReference grf;
				grf.prepare();
				Ilwis4GeorefConnector::loadMetaData(grf.ptr(), options, jraster["georeference"].toObject());

				raster->georeference(grf);
				raster->size(jraster["size"].toString());

				QJsonObject jdata = jraster["data"].toObject();

				DataDefinition def;
				Ilwis4Connector::loadDataDef(def, jdata);
				raster->datadefRef() = def;

				QJsonArray jbanddefs = jdata["banddefinitions"].toArray();
				for (int i = 0; i < raster->size().zsize(); ++i) {
					QJsonObject jbanddef = jbanddefs[i].toObject();
					Ilwis4Connector::loadDataDef(raster->datadefRef(i), jbanddef);
				}

				QJsonObject jstackDomain = jdata["stackdomain"].toObject();
				IDomain ilobj = Ilwis4DomainConnector::createDomain(options, jstackDomain);
				Ilwis4DomainConnector::loadMetaData(ilobj.ptr(), options, jstackDomain);
				QStringList indexes = jstackDomain["stackindexes"].toString().split("|");
				std::vector<QString> variants(indexes.size());
				for (int i = 0; i < indexes.size(); ++i) {
					variants[i] = indexes[i];
				}
				raster->stackDefinitionRef().setSubDefinition(ilobj, variants);

				QJsonValue v = jraster["attributes"];
				if (v != QJsonValue::Undefined) {
					QString url = v.toString();
					ITable tbl;
					if (tbl.prepare(url)) {
						raster->primaryKey(jraster["primarykey"].toString());
						raster->setAttributes(tbl);
					}
				}
			}
		}
	}
   
	return true;
}

bool Ilwis4RasterConnector::loadData(IlwisObject* obj, const IOOptions& options) {

	QFileInfo inf(sourceRef().toLocalFile());
	QString path = inf.absolutePath() + "/" + _datafile;
	if (!_dataRaster.isValid()){
		if (!_dataRaster.prepare(QUrl::fromLocalFile(path).toString())) {
			return false;
		}
	}
	if (obj->code().indexOf("band=") == 0) {
		int band = obj->code().mid(5).toInt();
		_dataRaster->resourceRef().addProperty("bandindex", band);
	}
	_dataRaster->constConnector()->loadData(obj, options);

	return true;
}
