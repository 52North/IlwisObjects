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
#include "itemdomain.h"
#include "representation.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "ilwis4connector.h"
#include "ilwis4domainconnector.h"
#include "ilwis4representationconnector.h"
#include "ilwisobjectconnector.h"
#include "factory.h"
#include "abstractfactory.h"


using namespace Ilwis;
using namespace Ilwis4C;

ConnectorInterface *Ilwis4RepresentationConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions& options)
{
	return new Ilwis4RepresentationConnector(resource, load, options);
}

Ilwis4RepresentationConnector::Ilwis4RepresentationConnector(const Ilwis::Resource &resource, bool load, const IOOptions& options) : Ilwis4Connector(resource, load, options)
{
	_version = 1;
}

bool Ilwis4RepresentationConnector::store(IlwisObject *obj, const IOOptions &options)
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

bool Ilwis4RepresentationConnector::store(IlwisObject *obj, const IOOptions& options, QJsonObject& jrpr) {

	Ilwis4Connector::store(obj, options, jrpr);

	Representation *rpr = static_cast<Representation *>(obj);

	QJsonObject jdom;
	Ilwis4DomainConnector::store(rpr->domain().ptr(), options, jdom);
	jrpr.insert("domain", jdom);

	if (rpr->domain().isValid() && hasType(rpr->domain()->ilwisType(), itITEMDOMAIN | itNUMERICDOMAIN)) {
		QString def = rpr->colors()->definition();
		jrpr.insert("definition", def);
	}

	return true;
}

bool Ilwis4RepresentationConnector::storeData(IlwisObject *obj, const IOOptions &options) {

	return true;
}

bool Ilwis4RepresentationConnector::loadMetaData(IlwisObject* obj, const IOOptions& options, const QJsonValue& jrpr) {

	Ilwis4Connector::loadMetaData(obj, options, jrpr);

	QJsonObject jdom = jrpr["domain"].toObject();
	IDomain dom = Ilwis4DomainConnector::createDomain(options, jdom);
	Ilwis4DomainConnector::loadMetaData(dom.ptr(), options, jdom);

	Representation *rpr = static_cast<Representation *>(obj);
	rpr->domain(dom);

	if (rpr->domain().isValid() && hasType(rpr->domain()->ilwisType(), itITEMDOMAIN | itNUMERICDOMAIN)) {
		QString def = jrpr["definition"].toString();
	
		rpr->colors()->fromDefinition(def);

	}
	   
	return true;
}

bool Ilwis4RepresentationConnector::loadMetaData(IlwisObject *obj, const IOOptions &options)
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

bool Ilwis4RepresentationConnector::loadData(IlwisObject* obj, const IOOptions& options) {
	return true;
}
