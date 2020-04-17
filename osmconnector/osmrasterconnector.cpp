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
#include "raster.h"
#include "ilwisobjectconnector.h"
#include "osmrasterconnector.h"
#include "ilwiscontext.h"
#include "ilwisobjectconnector.h"



using namespace Ilwis;
using namespace OSM;

ConnectorInterface *OSMRasterConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions& options)
{
	return new OSMRasterConnector(resource, load, options);
}

OSMRasterConnector::OSMRasterConnector(const Ilwis::Resource &resource, bool load, const IOOptions& options) : IlwisObjectConnector(resource, load, options)
{
	_version = 1;
}

IlwisObject *OSMRasterConnector::create() const {
	return new RasterCoverage(_resource);
}

bool OSMRasterConnector::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
	RasterCoverage *raster = static_cast<RasterCoverage *>(obj);
	QString server = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/backgroundmap-server", QString(""));
	QString name = server + "ilwis4_background_layer";
	ICoordinateSystem csy;
	csy.prepare("code=epsg:3857");
	csy->envelope(Envelope(Coordinate(0, 0), Coordinate(360, 85))); // fake bounds but valid

	IGeoReference grf;
	grf.prepare();
	grf->create("corners");
	grf->coordinateSystem(csy);
	grf->size(Size<>(256, 256, 1)); // fake size
	raster->georeference(grf, true);
	IDomain dom("code=domain:color");
	raster->setDataDefintions(dom, { 1 }, IDomain("count"));
	return true;
}

bool OSMRasterConnector::loadData(IlwisObject* obj, const IOOptions& options) {

	_binaryIsLoaded = false;
	if (!_dataRaster.isValid()) {
		if (!_dataRaster.prepare(source())) {
			return false;
		}
	}
	auto tilebb = _dataRaster->resourceRef()["tileboundingbox"].toString();
	QStringList tilebbs = tilebb.split(" ");
	if (tilebbs.size() != 5) {
		kernel()->issues()->log("Tile boundaries are not set correctly. OSM layer can not be retrieved:" + tilebb);
		return false;
	}
	
	tilebb.replace(" ", "_");
	QString basePath = context()->cacheLocation().toLocalFile();
	QString inputFileName = basePath + "/osm_mergedtiles_" + tilebb + ".png";
	if (QFile::exists(inputFileName)) {
		QUrl localUrl = QUrl::fromLocalFile(inputFileName);
		Resource res = obj->resource();
		res.setUrl(localUrl, true, false);
		res.setUrl(localUrl, false, false);
		res.addProperty("colormodel", "rgba");

		const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
		ConnectorInterface *connector = factory->createFromResource<>(res, "gdal");

		connector->loadData(obj, options);
		_binaryIsLoaded = true;
		emit finishedReadingData();
		RasterCoverage *raster = static_cast<RasterCoverage *>(obj);
		qDebug() << "SIZESSSS" << raster->size().toString() << raster->grid()->size().toString();
		return true;
	}

	return false;
}

QString OSMRasterConnector::provider() const
{
	return "osm";
}
