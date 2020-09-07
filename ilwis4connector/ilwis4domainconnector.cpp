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
#include "numericdomain.h"
#include "itemdomain.h"
#include "textdomain.h"
#include "colordomain.h"
#include "numericrange.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "itemrange.h"
#include "identifierrange.h"
#include "interval.h"
#include "intervalrange.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "ilwisobjectconnector.h"
#include "ilwis4connector.h"
#include "ilwis4domainconnector.h"
#include "ilwisobjectconnector.h"
#include "factory.h"
#include "abstractfactory.h"


using namespace Ilwis;
using namespace Ilwis4C;

ConnectorInterface *Ilwis4DomainConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions& options)
{
	return new Ilwis4DomainConnector(resource, load, options);
}

Ilwis4DomainConnector::Ilwis4DomainConnector(const Ilwis::Resource &resource, bool load, const IOOptions& options) : Ilwis4Connector(resource, load, options)
{
	_version = 1;
}

bool Ilwis4DomainConnector::store(IlwisObject *obj, const IOOptions &options)
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

IDomain Ilwis4DomainConnector::createDomain(const IOOptions& options, const QJsonObject& jdomain) {
	IDomain dom;
	QJsonObject base = jdomain["base"].toObject();
	IlwisTypes tp = TypeHelper::name2type(base["ilwistype"].toString());
	IlwisTypes vt = QString(jdomain["valuetype"].toString()).toULongLong();
	QString code = base["code"].toString();
	if (code == sUNDEF) {
		if (tp == itNUMERICDOMAIN) {
			dom = INumericDomain();
		}
		else if (tp == itITEMDOMAIN) {
			if (vt == itINDEXEDITEM) {
				IIndexedIdDomain d;
				d.prepare();
				dom = d;
			}
			else if (vt == itNAMEDITEM) {
				INamedIdDomain d;
				d.prepare();
				dom = d;
			}
			else if (vt == itTHEMATICITEM) {
				IThematicDomain d;
				d.prepare();
				dom = d;
			}
			else if (vt == itNUMERICITEM) {
				IIntervalDomain d;
				d.prepare();
				dom = d;
			}
			//dom.prepare();
		}
		else if (tp == itTEXTDOMAIN) {
			ITextDomain d;
			d.prepare();
			dom = d;
		}
	}
	else
		dom = IDomain(code, tp, options);
	//TODO colordomain

	return dom;


}
bool Ilwis4DomainConnector::store(IlwisObject *obj, const IOOptions& options, QJsonObject& jdomain) {
	Domain *dom = static_cast<Domain *>(obj);
	Ilwis4Connector::store(obj, options, jdomain);
	jdomain.insert("valuetype", QString::number(dom->valueType()));
	jdomain.insert("defaultrange", dom->range<Range>()->toString());

	return true;
}

bool Ilwis4DomainConnector::storeData(IlwisObject *obj, const IOOptions &options) {

	return true;
}

bool Ilwis4DomainConnector::loadMetaData(IlwisObject* object, const IOOptions& options, const QJsonValue& jvalue) {
	Ilwis4Connector::loadMetaData(object, options, jvalue);
	
	Domain * dom = static_cast<Domain *>(object);
	dom->range(Ilwis4DomainConnector::getRange(jvalue["defaultrange"].toString()));

	return true;
}

Ilwis::Range  *Ilwis4DomainConnector::getRange(const QString& rangedef) {
	
	QStringList parts = rangedef.split(":");
	if (parts[0] == "numericrange") {
		return new NumericRange(rangedef);
	}if (parts[0] == "indexedidentifierrange") {
		return new IndexedIdentifierRange(rangedef);
	}
	else if (parts[0] == "namedidentifierrange") {
		return new NamedIdentifierRange(rangedef);
	}
	else if (parts[0] == "thematicrange") {
		return new ThematicRange(rangedef);
	}
	else if (parts[0] == "intervalrange") {
		return new IntervalRange(rangedef);
	}
	else if (parts[0] == "colorpalette") {
		return new ColorPalette(rangedef);
	}

	return 0;
}
bool Ilwis4DomainConnector::loadMetaData(IlwisObject *obj, const IOOptions &options)
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

bool Ilwis4DomainConnector::loadData(IlwisObject* obj, const IOOptions& options) {
	return true;
}
