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
#include "ilwisdata.h"
#include "geometries.h"
#include "coveragedisplay\layermanager.h"
#include "coveragedisplay\layermodel.h"
#include "coveragedisplay\coveragelayermodel.h"
#include "coveragedisplay\visualattribute.h"
#include "georeference.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "raster.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "ilwiscontext.h"
#include "backgroundlayer.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_LAYERMODEL("backgroundlayer", BackgroundLayer)

BackgroundLayer::BackgroundLayer() : LayerModel()
{
}

BackgroundLayer::BackgroundLayer(LayerManager *manager, QStandardItem *parent, const IOOptions& options) : LayerModel(manager, parent, "Background", "determines the properties of the backgeround beneath the layer of maps", options)
{
    _isDrawable = true;
    _isValid = true;
    _icon = "vector.png";
    _layerType = itPOLYGONLAYER;
    active(false);
    order(10000);
    _backgroundcolor = QColor("red");

    fillData();

    add2ChangedProperties("buffers", true); // forces the calculation of the points through the prepare
}



bool BackgroundLayer::prepare(int prepType)
{
    auto PushPoints = [](double x, double y, double z, std::vector<qreal>& vector, std::vector<quint16>& indices)->void {
        vector.push_back(x);
        vector.push_back(y);
        vector.push_back(z);
        indices.push_back(indices.size());
    };

    if (hasType(prepType, LayerModel::ptGEOMETRY) && !isPrepared(LayerModel::ptGEOMETRY)) {

        Coordinate cmin = layerManager()->rootLayer()->zoomEnvelope().min_corner();
        Coordinate cmax = layerManager()->rootLayer()->zoomEnvelope().max_corner();

        std::vector<qreal> vertices;
        std::vector<quint16> indices;
        std::vector<qreal> colors;
        for (int i = 0; i < 6; ++i) {
            colors.push_back(_backgroundcolor.redF());
            colors.push_back(_backgroundcolor.greenF());
            colors.push_back(_backgroundcolor.blueF());
        }

        PushPoints(cmin.x, cmin.y, 0, vertices, indices);
        PushPoints(cmax.x, cmin.y, 0, vertices, indices);
        PushPoints(cmin.x, cmax.y, 0, vertices, indices);
        PushPoints(cmin.x, cmax.y, 0, vertices, indices);
        PushPoints(cmax.x, cmin.y, 0, vertices, indices);
        PushPoints(cmax.x, cmax.y, 0, vertices, indices);
        
        addVisualAttribute(new GlobalAttributeModel(TR("Background color"), "backgroundeditor", this));

        _buffer.addObject(0, vertices, indices, colors, itPOLYGON, 0);

        _prepared |= (LayerModel::ptGEOMETRY);

    }
	if (!_osmRaster.isValid()) {
		QString server = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/backgroundmap-server", QString(""));
		Resource res(itRASTER, QUrl(server + "ilwis4_background_layer"), QUrl(server));
		res.name("ilwis4_background_layer");

		_osmRaster.prepare(res);
		
		auto *rlayer = layerManager()->create(this, "rastercoverage", layerManager(), OSMLAYERNAME,"");
		CoverageLayerModel *covModel = static_cast<CoverageLayerModel *>(rlayer);
		covModel->coverage(_osmRaster);
		rlayer->add2ChangedProperties("buffers", true); // forces the layer to be added to the scene
	}
    return true;
}

QVariant BackgroundLayer::vproperty(const QString & key) const
{
    QVariant v = LayerModel::vproperty(key);

    return v;
}

void BackgroundLayer::vproperty(const QString & key, const QVariant & value)
{
    LayerModel::vproperty(key, value);
	if (key == "zoomchanged") {
		updateOSMRaster();
	}
}

LayerModel *BackgroundLayer::create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options)
{
    return new BackgroundLayer(manager, parentLayer, options);
}

QVector<qreal> BackgroundLayer::vertices(qint32 bufferIndex, const QString&) const {
    if (bufferIndex <_buffer.bufferCount())
        return _buffer.buffer(bufferIndex)->_vertices;
    return QVector < qreal >();
}

QVector<int> BackgroundLayer::indices(qint32 bufferIndex, const QString&) const {
    if (bufferIndex < _buffer.bufferCount())
        return _buffer.buffer(bufferIndex)->_indices;
    return QVector<int>();
}

int BackgroundLayer::numberOfBuffers(const QString &type) const
{
    if (type == "polygons")
        return _buffer.bufferCount();
    return 0;
}

bool BackgroundLayer::renderReady() {
    return true; // TODO: return the correct value
}

void BackgroundLayer::renderReady(bool yesno) {
    // TODO
}

void BackgroundLayer::updateOSMRaster() {
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
	auto *layer = layerManager()->findLayerByName(OSMLAYERNAME);
	if (_osmRaster.isValid() && layer && layer->active()) {
	

		Envelope env = layerManager()->rootLayer()->zoomEnvelope();
		BoundingBox bb = layerManager()->rootLayer()->screenGrf()->coord2Pixel(env);

		Envelope llEnv = CoordinateSystem::latLonEnvelope(layerManager()->rootLayer()->screenCsy(), env);
		Envelope pmEnv = _osmRaster->coordinateSystem()->convertEnvelope(layerManager()->rootLayer()->screenCsy(), env);

		Envelope envWorld({ -20026376.39, -20048966.10 }, { 20026376.39, 20048966.10 });
		double zoomSize = std::min(pmEnv.xlength(), pmEnv.ylength());
		int z = std::ceil(std::log(envWorld.xlength() / zoomSize) / log(2)) + 1;
		z = std::min(z, 18);

		int tileMinX = ftilex(llEnv.min_corner(), z);
		int tileMinY = ftiley(llEnv.min_corner(), z);

		int tileMaxX = ftilex(llEnv.max_corner(), z);
		int tileMaxY = ftiley(llEnv.max_corner(), z);

		BoundingBox tilesBB({ tileMinX, tileMinY }, { tileMaxX, tileMaxY });

		Coordinate llMin = llTile(tilesBB.min_corner(), z);
		Coordinate llMax = llTile({ tilesBB.max_corner().x + 1, tilesBB.max_corner().y + 1}, z);
		Envelope tileEnvelope(llMin, llMax);
		ICoordinateSystem csyWgs84("code=epsg:4326");

		env = _osmRaster->coordinateSystem()->convertEnvelope(csyWgs84, tileEnvelope);

		qDebug() << llMin.toString() << llMax.toString() << tilesBB.toString() << "|" << env.toString();

		_osmRaster->resourceRef().addProperty("tileboundingbox", tilesBB.toString() + " " + QString::number(z));
		_osmRaster->size({ tilesBB.size().xsize() * 256, tilesBB.size().ysize() * 256, 1 });
		_osmRaster->coordinateSystem()->envelope(env);
		_osmRaster->georeference()->as<CornersGeoReference>()->internalEnvelope(env);
		_osmRaster->georeference()->compute();
	
		QString server = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/backgroundmap-server", QString(""));
		_downloadQueue.clear();

		int startTileX = tilesBB.min_corner().x;
		int startTileY = tilesBB.min_corner().y;
		int endTileX = tilesBB.max_corner().x;
		int endTileY = tilesBB.max_corner().y;
		QStringList requests;
		for (int y = startTileY; y <= endTileY; ++y) {
			for (int x = startTileX; x <= endTileX; ++x) {
				QString request = server + QString::number(z) + "/" + QString::number(x) + "/" + QString::number(y) + ".png";
				QUrl url(request);
				_downloadQueue.enqueue(url);

			}

		}
		_osmInputFiles = "";
		getData();
	}

}

void BackgroundLayer::downloadFinished() {

	_output.close();

	if (_currentDownload->error()) {
		// download failed
		kernel()->issues()->log(QString("OSM tile download Failed: %1\n").arg(qPrintable(_currentDownload->errorString())));
	}
	_currentDownload->deleteLater();
	getData();
}

void BackgroundLayer::downloadReadyRead()
{
	_output.write(_currentDownload->readAll());
}

QString BackgroundLayer::makeFileName(const QString& url) const {

	QStringList parts = url.split("/");
	QString py = parts[parts.size() - 1];
	QString px = parts[parts.size() - 2];
	QString pz = parts[parts.size() - 3];

	QString basePath = context()->cacheLocation().toLocalFile();

	QString basename = basePath + "/osm_" + pz + "_" + px + "_" + py;


	return basename;
}

void BackgroundLayer::getData() {
	auto *layer = layerManager()->findLayerByName(OSMLAYERNAME);
	if (_downloadQueue.size() > 0) {
		layer->active(false);
		QUrl url = _downloadQueue.dequeue();
		QString fname = makeFileName(url.toString());
		if (_osmInputFiles != "")
			_osmInputFiles += QString("|");
		_osmInputFiles += fname;

		if (!QFile::exists(fname)) {
			_output.setFileName(fname);
			_output.open(QFile::WriteOnly);

			QNetworkRequest nrequest(url);
			_currentDownload = _manager.get(nrequest);

			connect(_currentDownload, &QNetworkReply::finished, this, &BackgroundLayer::downloadFinished);
			connect(_currentDownload, &QNetworkReply::readyRead, this, &BackgroundLayer::downloadReadyRead);
		}
		else {
			getData();
		}
	}
	else {
		
		if (layer) {
			QString tilebb = _osmRaster->resourceRef()["tileboundingbox"].toString();
			tilebb.replace(" ", "_");
			QString basePath = context()->cacheLocation().toLocalFile();
			QString outputFileName = basePath + "/osm_mergedtiles_" + tilebb + ".png";
			int xsize = _osmRaster->size().xsize() / 256;
			int ysize = _osmRaster->size().ysize() / 256;

			QString expr = QString("mergepictures(%1, '%2', %3, %4)").arg(outputFileName).arg(_osmInputFiles).arg(xsize).arg(ysize);

			ExecutionContext ctx;
			commandhandler()->execute(expr, &ctx);

			layer->vproperty("calcdimensions");
			layer->add2ChangedProperties("buffers", true); // forces the calculation of the points through the prepare
			layer->vproperty("updatetextures", "dummy");
			const IlwisObjectConnector *ioObjectConnector = static_cast<const IlwisObjectConnector *>(_osmRaster->constConnector().get());
			connect(ioObjectConnector, &IlwisObjectConnector::finishedReadingData, this, &BackgroundLayer::activateLayer);
		}
	}
}

void BackgroundLayer::activateLayer() {
	auto *layer = layerManager()->findLayerByName(OSMLAYERNAME);
	layer->active(true);
}

