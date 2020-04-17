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
#include <QDomDocument>
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
#include "sentinelrasterconnector.h"
#include "catalogexplorer.h"
#include "sentinelcatalogexplorer.h"
#include "ilwisobjectconnector.h"



using namespace Ilwis;
using namespace Sentinel;

ConnectorInterface *SentinelRasterConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions& options)
{
	return new SentinelRasterConnector(resource, load, options);
}

SentinelRasterConnector::SentinelRasterConnector(const Ilwis::Resource &resource, bool load, const IOOptions& options) : IlwisObjectConnector(resource, load, options)
{
	_version = 1;
}

IlwisObject *SentinelRasterConnector::create() const {
	return new RasterCoverage(_resource);
}

bool SentinelRasterConnector::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
	QString csyCode = obj->resourceRef()["coordinatesystem"].toString();
	ICoordinateSystem csy;
	if (!csy.prepare(csyCode))
		return false;
	RasterCoverage *raster = static_cast<RasterCoverage *>(obj);
	raster->coordinateSystem(csy);

	IGeoReference grf;
	Resource& res = obj->resourceRef();
	QString grfP = "code=georef:" + res["georeference"].toString();
	if (!grf.prepare(grfP))
		return false;
	raster->georeference(grf);
	std::vector<double> bands = { 1 };
	raster->setDataDefintions({ "code=domain:value" }, bands);
	QString rootConfig = raster->resourceRef()["rootxml"].toString();
	QDomDocument doc("sentinel2");
	QFile file(rootConfig);
	if (!file.open(QIODevice::ReadOnly)) {
		kernel()->issues()->log("Root config file for sentinel2 not found: ", rootConfig);
		return false;
	}if (!doc.setContent(&file)) {
		file.close();
		return false;
	}
	file.close();

	QString product;
	QDomElement root = doc.documentElement();
	QString startTag = root.tagName();
	product = "1C";

	if (startTag == "n1:Level-2A_User_Product")
		product = "2A";

	//QString tag = SentinelCatalogExplorer::_tagNames.at(product + "ProductInfo");
	//QString tagPath = "n1:General_Info/Product_Info/PROCESSING_LEVEL";
	//QString tagValue = SentinelCatalogExplorer::tagValue(tagPath, root);

   
	return true;
}

bool SentinelRasterConnector::loadData(IlwisObject* obj, const IOOptions& options) {

	if (!_binaryIsLoaded) {
		Resource res = obj->resource();
		if (!res.hasProperty("datafile"))
			return false;
		QUrl dataFile(res["datafile"].toString());
		QString localFile = dataFile.toLocalFile();
		if (!_dataRaster.isValid()) {
			if (!_dataRaster.prepare(dataFile.toString())) {
				return false;
			}
			_dataRaster->resourceRef().addProperty("scale", 0.001);
			_dataRaster->resourceRef().addProperty("offset", 0);
			_dataRaster->resourceRef().addProperty("undefined", 0);
		}
		_dataRaster->constConnector()->loadData(obj, options);
	}

	return true;
}

QString SentinelRasterConnector::provider() const
{
	return "Ilwis4";
}
