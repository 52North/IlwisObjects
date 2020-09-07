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
#include "operationmetadata.h"
#include "script.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "ilwis4connector.h"
#include "ilwis4scriptconnector.h"
#include "ilwisobjectconnector.h"
#include "factory.h"
#include "abstractfactory.h"


using namespace Ilwis;
using namespace Ilwis4C;

ConnectorInterface *Ilwis4ScriptConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions& options)
{
	return new Ilwis4ScriptConnector(resource, load, options);
}

Ilwis4ScriptConnector::Ilwis4ScriptConnector(const Ilwis::Resource &resource, bool load, const IOOptions& options) : Ilwis4Connector(resource, load, options)
{
	_version = 1;
}

bool Ilwis4ScriptConnector::store(IlwisObject *obj, const IOOptions &options)
{
	QJsonArray objects;
	QJsonObject jroot, jdomain;

	IOOptions newOptions = options;
	newOptions.addOption("version", _version);

	store(obj, newOptions, jdomain);
	jroot.insert("ilwisobject", jdomain);

	objects.append(jroot);

	flush(obj, objects);

	storeData(obj, options);

	return true;
}

bool Ilwis4ScriptConnector::store(IlwisObject *obj, const IOOptions& options, QJsonObject& jscript) {
	
	Ilwis4Connector::store(obj, options, jscript);
	Script *script = static_cast<Script *>(obj);
	jscript.insert("text", script->text());
	return true;
}

bool Ilwis4ScriptConnector::storeData(IlwisObject *obj, const IOOptions &options) {

	return true;
}

bool Ilwis4ScriptConnector::loadMetaData(IlwisObject* obj, const IOOptions& options, const QJsonValue& jvalue) {
	Ilwis4Connector::loadMetaData(obj, options, jvalue);

	QString text = jvalue["text"].toString();
	Script *script = static_cast<Script *>(obj);
	script->text(text);

	return true;
}

bool Ilwis4ScriptConnector::loadMetaData(IlwisObject *obj, const IOOptions &options)
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


	return true;
}

bool Ilwis4ScriptConnector::loadData(IlwisObject* obj, const IOOptions& options) {
	return true;
}
