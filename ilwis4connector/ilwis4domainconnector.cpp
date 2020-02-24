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
}

bool Ilwis4DomainConnector::store(IlwisObject *obj, const IOOptions &options)
{
	QJsonObject jroot;

	store(obj, options, jroot);

	flush(obj, jroot);
	return true;
}

bool Ilwis4DomainConnector::store(IlwisObject *obj, const IOOptions& options, QJsonObject& jdomain) {
	Domain *dom = static_cast<Domain *>(obj);
	Ilwis4Connector::store(obj, options, jdomain);
	jdomain.insert("valuetype", TypeHelper::type2name(dom->valueType()));
	jdomain.insert("defaultrange", dom->range<Range>()->toString());

	return true;
}


bool Ilwis4DomainConnector::storeData(IlwisObject *obj, const IOOptions &options) {

	return true;
}

bool Ilwis4DomainConnector::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
	if (!Ilwis4Connector::loadMetaData(obj, options))
		return false;


	return true;
}

bool Ilwis4DomainConnector::loadData(IlwisObject* obj, const IOOptions& options) {
	Table *tbl = static_cast<Table *>(obj);

	return true;
}
