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
#include "table.h"
#include "ilwisobjectconnector.h"
#include "ilwis4connector.h"
#include "ilwis4tableconnector.h"
#include "ilwisobjectconnector.h"
#include "factory.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorfactory.h"


using namespace Ilwis;
using namespace Ilwis4C;

ConnectorInterface *Ilwis4TableConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions& options)
{
    return new Ilwis4TableConnector(resource, load, options);
}

Ilwis4TableConnector::Ilwis4TableConnector(const Ilwis::Resource &resource, bool load, const IOOptions& options) : Ilwis4Connector(resource, load, options)
{
	_version = 1;
}

bool Ilwis4TableConnector::store(IlwisObject *obj, const IOOptions &options) 
{
	QJsonArray objects;
	QJsonObject jroot, jtable;

	IOOptions newOptions = options;
	newOptions.addOption("version", _version);

	store(obj, newOptions, jtable);
	jroot.insert("ilwisobject", jtable);
	objects.append(jroot);

	flush(obj, objects);

	storeData(obj, options);

	return true;
}
bool Ilwis4TableConnector::store(IlwisObject *obj, const IOOptions& options, QJsonObject& jtable) {
	Table *tbl = static_cast<Table *>(obj);

	Ilwis4Connector::store(obj, options, jtable);

	jtable.insert("columns", (int)tbl->columnCount());
	jtable.insert("rows", (int)tbl->recordCount());
	QJsonArray defs;
	for (int col = 0; col < tbl->columnCount(); ++col) {
		const ColumnDefinition& coldef = tbl->columndefinitionRef(col);
		QJsonObject jcolumnDef;
		jcolumnDef.insert("name", coldef.name());
		QJsonObject jdatadef;
		storeDataDef(coldef.datadef(), jdatadef);
		jcolumnDef.insert("datadefinition", jdatadef);
		defs.append(jcolumnDef);
	}
	jtable.insert("columndefinitions", defs);
	QString path = obj->resource().url(true).toLocalFile();
	QFileInfo inf(path);
	jtable.insert("binarydata", inf.baseName() + ".ods#");
	return true;
}

bool Ilwis4TableConnector::storeData(IlwisObject *obj, const IOOptions &options ){
	
	const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
	if (!factory) {
		kernel()->issues()->log("Couldn't find factory for gdal connector");
		return false;
	}

	ConnectorInterface *connector = factory->createFromResource<>(obj->resource(), "spreadsheets", { "format","ods" });
	if (connector) {
		connector->format("ods");
		connector->store(obj);
		QString path = obj->resource().url(true).toLocalFile();
		QString pathOds = path;
		pathOds = pathOds.replace(".ilwis4", ".ods");
		path = path.replace(".ilwis4", ".ods_");

		QFile::rename(pathOds, path);
		delete connector;
		return true;
	}


    return false;
}

bool Ilwis4TableConnector::loadMetaData(IlwisObject *obj, const IOOptions &options)
{

	QFileInfo inf = _resource.url(true).toLocalFile();
	QFile file;
	file.setFileName(inf.absoluteFilePath());
	if (file.open(QIODevice::ReadOnly)) {
		QString meta = file.readAll();
		QJsonDocument doc = QJsonDocument::fromJson(meta.toUtf8());
		if (!doc.isNull()) {
			Table *table = static_cast<Table *>(obj);
			QJsonArray jobjects = doc.array();
			QJsonValue jvalue = jobjects.at(0);
			QJsonValue jtable = jvalue["ilwisobject"];



			int idx = 0;
			QJsonArray jdefs = jtable["columndefinitions"].toArray();
			for (auto jdefr : jdefs) {
				QJsonObject jdef = jdefr.toObject();
				QString cname = jdef["name"].toString();
				DataDefinition def;
				loadDataDef(def, jdef["datadefinition"].toObject());
				table->addColumn(ColumnDefinition( cname, def, idx++ ));
			}
			int rows = jtable["rows"].toInt();
			table->recordCount(rows);
		}
	}
    return true;
}

bool Ilwis4TableConnector::loadData(IlwisObject* obj, const IOOptions& options){

	if (!dataIsLoaded()) {
		const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
		if (!factory) {
			kernel()->issues()->log("Couldn't find factory for gdal connector");
			return false;
		}
		Resource res = obj->resource();
		QString path = res.url(true).toString();
		path = path.replace(".ilwis4", ".ods_");
		res.setUrl(path, true, false);
		res.setUrl(path, false, false);

		ConnectorInterface *connector = factory->createFromResource<>(res, "spreadsheets", { "format" , "ods_" });
		connector->format("ods");
		auto opt = options;
		opt.addOption({ "nometadata",true });
		opt.addOption({ "headerline", 0 });
		connector->loadData(obj, opt);

		_binaryIsLoaded = connector->dataIsLoaded();
		delete connector;
	}
  
    return true;
}
