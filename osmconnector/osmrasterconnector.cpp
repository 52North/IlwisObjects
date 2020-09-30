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

	auto ftilex = [](const Coordinate& ll, int zoomLevel) ->int {
		return (int)(floor((ll.x + 180.0) / 360.0 * (1 << zoomLevel)));
	};

	auto ftiley = [](const Coordinate& ll, int zoomLevel) ->int {
		double latrad = ll.y * M_PI / 180.0;
		return (int)(floor((1.0 - asinh(tan(latrad)) / M_PI) / 2.0 * (1 << zoomLevel)));
	};

	auto llTile = [](const Pixel& tile, int zoomLevel) -> Coordinate {
		double f = (double)tile.x / (double)(1 << zoomLevel);
		double lonLeft = f * 360.0 - 180;
		double n = M_PI - 2.0 * M_PI * tile.y / (double)(1 << zoomLevel);
		double latBottom = 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));

		return Coordinate(lonLeft, latBottom);
	};

	qDebug() << "calling loaddata";
	if (!_dataRaster.isValid()) {
		if (!_dataRaster.prepare(source())) {
			return false;
		}

		//vsicurl/ftp://user:password@example.com/foldername/file.zip/example.shp
		auto bb = _dataRaster->resourceRef()["tileboundingbox"].toString();
		QStringList tilebb = bb.split(" ");
		if (tilebb.size() != 5) {
			kernel()->issues()->log("Tile boundaries are not set correctly. OSM layer can not be retrieved:" + bb);
			return false;
		}
			   		 	
		Resource res = obj->resource();
		QString path = res.url(true).toString();
		res.setUrl(path, true, false);
		res.setUrl(path, false, false);
		QFileInfo inf(res.url(true).toLocalFile());
		QString server = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/backgroundmap-server", QString(""));

		const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
		ConnectorInterface *connector = factory->createFromResource<>(res, "gdal");
		int startTileX = tilebb[0].toInt();
		int startTileY = tilebb[1].toInt();
		int endTileX = tilebb[2].toInt();
		int endTileY = tilebb[3].toInt();
		int zoom = tilebb[4].toInt();
		for (int y = startTileY; y < endTileY; ++y) {
			for (int x = startTileX; x < endTileX; ++x) {
				QString request = server + QString::number(zoom) + "/" + QString::number(x) + "/" + QString::number(y) + ".png";
				QString gdalReq = "vsicurl/" + request;
				IOOptions opt = options;
				opt.addOption("vsicurl", gdalReq);
				connector->loadData(obj, opt);
			}
		}


		return true;
	}
	return false;
}

QString OSMRasterConnector::provider() const
{
	return "osm";
}
