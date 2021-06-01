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

#include <QBuffer>
#include <QColor>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include "raster.h"
#include "version.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "ilwis4connector.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "featurecoverage.h"
#include "numericdomain.h"
#include "domainitem.h"
#include "identifieritem.h"
#include "catalog.h"
#include "thematicitem.h"
#include "interval.h"
#include "coloritem.h"
#include "identifierrange.h"
#include "intervalrange.h"
#include "colorrange.h"
#include "itemdomain.h"
#include "coordinatesystem.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "proj4parameters.h"
#include "conventionalcoordinatesystem.h"
#include "feature.h"
#include "factory.h"
#include "abstractfactory.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "location.h"
#include "workflownode.h"
#include "workflow.h"
#include "representation.h"
#include "analysispattern.h"
#include "applicationmodel.h"
#include "combinationmatrix.h"
#include "ilwis4domainconnector.h"
#include "model.h"



using namespace Ilwis;
using namespace Ilwis4C;

ConnectorInterface *Ilwis4Connector::create(const Resource &resource, bool load, const IOOptions &options) {
    return new Ilwis4Connector(resource, load, options);

}

IlwisObject *Ilwis4Connector::create() const
{
    switch(_resource.ilwisType()){
    case itFEATURE:
        return new FeatureCoverage(_resource);
    case itRASTER:
        return new RasterCoverage(_resource);
    case itTABLE:
    case itFLATTABLE:
        return new FlatTable(_resource);
    case itNUMERICDOMAIN:
        return new NumericDomain(_resource);
    case itGEOREF:
        return new GeoReference(_resource);
    case itWORKFLOW:
        return new Workflow(_resource);
    case itCONVENTIONALCOORDSYSTEM:
        return new ConventionalCoordinateSystem(_resource);
     case itCATALOG:
        return new Catalog(_resource);
    case itMODEL:
        return new Model(_resource);
    case itCOMBINATIONMATRIX:
        return new CombinationMatrix(_resource) ;
    case itITEMDOMAIN:{
        if ( hasType(_resource.extendedType(), itNUMERICITEM))
            return new IntervalDomain(_resource);
        if ( hasType(_resource.extendedType(), itIDENTIFIERITEM))
            return new NamedIdDomain(_resource);
        if ( hasType(_resource.extendedType(), itTHEMATICITEM))
            return new ThematicDomain(_resource);
        if ( hasType(_resource.extendedType(), itPALETTECOLOR))
            return new ItemDomain<ColorItem>(_resource);

    }
	case itREPRESENTATION:
		return new Representation(_resource);
    default:
        return 0;
    }
}


Ilwis4Connector::Ilwis4Connector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource,load,options)
{
    QUrlQuery query(sourceRef().url());
    QString url =QString(sourceRef().url(true).toString(QUrl::RemoveQuery));
    if ( query.hasQueryItem("datasource")){
        QString item = query.queryItemValue("datasource");
        url += "/" + item;
        sourceRef().setUrl(url);
    }
    if ( url.indexOf(".ilwis4") == -1){
        url += ".ilwis4";
        sourceRef().setUrl(url);
        sourceRef().setUrl(url,true);
    }
}

Ilwis4Connector::~Ilwis4Connector()
{

}

int Ilwis4Connector::loadMetaData(IlwisObject *object, const IOOptions &options, const QJsonValue& jvalue)
{

	QJsonObject obj = jvalue.toObject();
	QJsonValue jbase = obj.value("base");
	object->code(toString(jbase, "code"));
	object->createTime(toString(jbase, "creationdate"));
	object->modifiedTime(toString(jbase, "modifieddate"));
	object->extendedType(toString(jbase, "extendedtype").toULongLong());
	object->readOnly(toBool(jbase, "logicalreadonly"));
	object->name(toString(jbase, "name"));
	QJsonValue jv = jbase["longname"];
	if (jv != QJsonValue::Undefined) {
		object->resourceRef().addProperty("longname", jv.toString());
	}
	jv = jbase["storename"];
	if (jv != QJsonValue::Undefined) {
		object->resourceRef().addProperty("storename", jv.toString());
	}

	QJsonValue context = obj.value("context");
	if (!context.isUndefined()) {
		QJsonArray metaobjects = obj.value("metadata").toArray();
		for (auto&& metat : metaobjects) {
			QJsonObject meta = metat.toObject();
			QStringList keys = meta.keys();
			for (auto key : keys) {
				QJsonValue v = meta[key];
				object->resourceRef().addMetaTag(key, v.toString());
			}

		}
		object->setDescription(obj.value("description").toString());
	}
	return jvalue["version"].toInt();
}

bool Ilwis4Connector::loadData(IlwisObject *object, const IOOptions &options){
	return true;
}




bool Ilwis4Connector::store(IlwisObject *obj, const IOOptions &options, QJsonObject& jroot) {

	Resource res = obj->resource(IlwisObject::cmOUTPUT);
	bool isSupport = options.contains("status") ? options["status"].toString() == "support" : false;

	//QJsonDocument doc;
	QJsonObject jbase, jcontext;
	if (!isSupport) {
		jbase.insert("creationdate", Time(res.createTime()).toString());
		jbase.insert("modifieddate", Time(res.modifiedTime()).toString());
	}
	jbase.insert("logicalreadonly", obj->isReadOnly());
	jbase.insert("ilwistype", TypeHelper::type2name(obj->ilwisType()));

	if (res.hasProperty("longname")) {
		jbase.insert("longname", res["longname"].toString());
	}

	if (res.hasProperty("storename")) {
		jbase.insert("storename", res["storename"].toString());
	}
	QString nm = res.name();
	nm.remove(".ilwis4");
	jbase.insert("name", nm);
	if (!isSupport)
		jbase.insert("extendedtype", QString::number(obj->extendedType()));
	jbase.insert("code", obj->code());

	if (!isSupport) {
		auto metadata = res.metadata();
		if (metadata.size() > 0) {
			QJsonArray jsmetadata;
			for (auto item : metadata) {
				QJsonObject data;
				data.insert(item.first, item.second);
				jsmetadata.append(data);
			}
			jcontext.insert("metadata", jsmetadata);
		}
		jcontext.insert("description", obj->description());
	}

	jroot.insert("base", jbase);

	if (!isSupport) {
		QString id = QUuid::createUuid().toString() + "/" + obj->name();
		jroot.insert("aid", id);
		jroot.insert("context", jcontext);
	}
	jroot.insert("version", options["version"].toInt());
	
	return true;

}
void Ilwis4Connector::flush(const IlwisObject *obj, const QJsonArray& jobjects) {
	QFile file;
	Resource res = obj->resource(IlwisObject::cmOUTPUT);
	file.setFileName(res.url(true).toLocalFile());

	auto doc = QJsonDocument(jobjects);
	auto bytes = doc.toJson();
	if (file.open(QIODevice::WriteOnly)) {
		file.write(bytes);
		file.close();
	}
}

void Ilwis4Connector::storeDataDef(const DataDefinition& def, QJsonObject& jdatadef)  {
	QJsonObject jdom;
	Ilwis4DomainConnector::store(def.domain().ptr(), { "status", "support" }, jdom);
	jdatadef.insert("domain", jdom);
	jdatadef.insert("actualrange", def.range<Range>()->toString());
}

void Ilwis4Connector::loadDataDef(DataDefinition& def, QJsonObject& jdatadef) {
	IDomain ilobj = Ilwis4DomainConnector::createDomain(IOOptions(), jdatadef["domain"].toObject());
	Ilwis4DomainConnector::loadMetaData(ilobj.ptr(), IOOptions(), jdatadef["domain"].toObject());
	QString rangedef = jdatadef["actualrange"].toString();
	def.domain(ilobj);
	def.range(Ilwis4DomainConnector::getRange(rangedef));
}

void Ilwis4Connector::store(const QString& v, IlwisTypes valueType, QJsonObject& jroot, const IOOptions &options) {

	if (hasType(valueType, itILWISOBJECT)) {
		bool storeall = options.contains("storeall") ? options["storall"].toBool() : false;
		jroot.insert("storeall", storeall);
		if (storeall) {
			IIlwisObject obj;
			obj.prepare(v, valueType);
			if (obj.isValid()) {
				QJsonObject jobj;
				IOOptions opt = options;
				opt.addOption({"status", "support"});
				Ilwis4Connector::store(obj.ptr(), opt, jobj);
				jroot.insert("fixed", jobj);
			}
		}else
			jroot.insert("fixed", v);
	}
	else {
		jroot.insert("fixed", v);
	}
}

bool Ilwis4Connector::loadMetaData(const QJsonObject& jvalue, IlwisTypes tp, QString& v)
{
	if (hasType(tp, itILWISOBJECT)) {
		bool storeall = jvalue["storeall"].toBool();
		if (storeall) {
			// :TODO			
		}
		else {
			v = jvalue["fixed"].toString();
		}
	}
	else {
		v = jvalue["fixed"].toString();
	}
	return true;
}

QString Ilwis4Connector::provider() const
{
    return "Ilwis4";
}


bool Ilwis4Connector::dataIsLoaded() const
{
    return _binaryIsLoaded;
}

QString Ilwis4Connector::format() const
{
    return IlwisObject::type2Name(source().ilwisType());
}

