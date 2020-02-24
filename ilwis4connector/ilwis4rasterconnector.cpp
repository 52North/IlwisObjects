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
#include "raster.h"
#include "ilwisobjectconnector.h"
#include "ilwis4connector.h"
#include "ilwis4rasterconnector.h"
#include "ilwis4coordinatesystemconnector.h"
#include "ilwis4georefconnector.h"
#include "ilwis4domainconnector.h"
#include "ilwis4tableconnector.h"
#include "ilwisobjectconnector.h"
#include "factory.h"
#include "abstractfactory.h"


using namespace Ilwis;
using namespace Ilwis4C;

ConnectorInterface *Ilwis4RasterConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions& options)
{
	return new Ilwis4RasterConnector(resource, load, options);
}

Ilwis4RasterConnector::Ilwis4RasterConnector(const Ilwis::Resource &resource, bool load, const IOOptions& options) : Ilwis4Connector(resource, load, options)
{
}

bool Ilwis4RasterConnector::store(IlwisObject *obj, const IOOptions &options)
{
	QJsonObject jroot;

	store(obj, options, jroot);

	flush(obj, jroot);

	return true;
}
bool Ilwis4RasterConnector::store(IlwisObject *obj, const IOOptions& options, QJsonObject& jraster) {

	auto opt = options;
	opt["status"] = "support";
	RasterCoverage *raster = static_cast<RasterCoverage *>(obj);
	Ilwis4Connector::store(obj, options, jraster);
	QJsonObject jgrf;
	Ilwis4GeorefConnector::store(raster->georeference().ptr(), opt, jgrf);
	QJsonObject jcsy;
	Ilwis4CoordinateSystemConnector::store(raster->coordinateSystem().ptr(), opt, jcsy);

	jraster.insert("georeference", jgrf);
	jraster.insert("coordinatesystem", jcsy);

	QJsonObject jdata;
	storeDataDef(raster->datadef(), jdata);

	QJsonArray jbandDefs;
	for (int index = 0; index < raster->size().zsize(); ++index) {
		const DataDefinition& def = raster->datadef(index);
		QJsonObject jdatadef;
		storeDataDef(def, jdatadef);
		jbandDefs.append(jdatadef);
	}
	jdata.insert("banddefinition", jbandDefs);

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
	if (raster->hasAttributes()) {
		Ilwis4TableConnector::store(raster->attributeTable().ptr(), options, jattributes);
		jraster.insert("attributes", jattributes);
		jraster.insert("primarykey", raster->primaryKey());
	}
	

	return true;
}

bool Ilwis4RasterConnector::storeData(IlwisObject *obj, const IOOptions &options) {

	return true;
}

bool Ilwis4RasterConnector::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
	if (!Ilwis4Connector::loadMetaData(obj, options))
		return false;


	return true;
}

bool Ilwis4RasterConnector::loadData(IlwisObject* obj, const IOOptions& options) {

	return true;
}
