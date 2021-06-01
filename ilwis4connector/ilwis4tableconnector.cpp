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
#include "operationhelper.h"
#include "factory.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "qtcsv/include/writer.h"
#include "qtcsv/include/reader.h"
#include "qtcsv/include/variantdata.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#include "quazip//quazipdir.h"
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
	QString path = obj->resource(IlwisObject::cmOUTPUT).url(true).toLocalFile();
	QFileInfo inf(path);
	jtable.insert("binarydata", inf.baseName() + ".izip");
	return true;
}

bool Ilwis4TableConnector::storeData(IlwisObject *obj, const IOOptions &options ){
	
	QFileInfo fi(_resource.url(true).toLocalFile());
	QString basename = fi.baseName();
	QString path = fi.absolutePath() + "/" + basename + ".csv_";
	
	QtCSV::Writer writer;
	QtCSV::VariantData varData;
	Table *tbl = static_cast<Table *>(obj);
	for (int rec = 0; rec < tbl->recordCount(); ++rec) {
		Record record = tbl->record(rec);
		QVariantList data;
		Ilwis::CRecordIter begin(record.cbegin()), end(record.cend());
		while (begin != end) {
			data << *begin;
			++begin;
		}
		varData << data;

	}
	if (false == QtCSV::Writer::write(path, varData))
	{
		kernel()->issues()->log(TR("Couldn't write file:" + path));
		return false;
	}
	QFile file(path);
	if (file.open(QIODevice::ReadOnly)) {
		QString zipPath = path;
		zipPath.replace(".csv_", ".izip");
		QuaZip zipfile(zipPath);
		zipfile.open(QuaZip::mdCreate);
		QuaZipFile filez(&zipfile);
		filez.open(QIODevice::WriteOnly, QuaZipNewInfo(_resource.name()));

		QTextStream ts(&file);
		filez.write(ts.readAll().toUtf8());

		filez.close();
		zipfile.close();
		file.remove();

	}
	return true;;
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

	_binaryIsLoaded = false;
	Resource res = obj->resource();
	QString path = res.url(true).toLocalFile();
	QString zipPath = path.replace(".ilwis4", ".izip");

	QuaZip zipfile(zipPath);
	zipfile.open(QuaZip::mdUnzip);
	QuaZipFile file(&zipfile);

	Table *tbl = static_cast<Table *>(obj);
	for (bool f = zipfile.goToFirstFile(); f; f = zipfile.goToNextFile()) {
		file.open(QIODevice::ReadOnly);
		QtCSV::Reader reader;
		int recCount = 0;
		QList<QStringList> data = reader.readToList(file);
		for (auto rec : data) {
			if (rec.size() == tbl->columnCount()) {
				for (quint32 c = 0; c < tbl->columnCount(); ++c) {
					const ColumnDefinition& def = tbl->columndefinitionRef(c);
					if (hasType(def.datadef().domain()->ilwisType(), itNUMERICDOMAIN | itITEMDOMAIN)) {
						tbl->setCell(c, recCount, OperationHelper::unquote(rec[c]).toDouble(),true);
					}
					else {
						tbl->setCell(c, recCount, OperationHelper::unquote(rec[c]), true);
					}
				}
			}
			++recCount;
		}

		file.close();
		_binaryIsLoaded = true;
	}
	return _binaryIsLoaded;
}
