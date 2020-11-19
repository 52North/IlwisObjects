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

#pragma once

#include <QObject>
#include <QVariant>
#include <QtNetwork>
#include <QtCore>
#include <QColor>
#include "box.h"
#include "ilwisdata.h"
#include "coveragedisplay\layermodel.h"
#include "coveragedisplay\oglbuffer.h"

namespace Ilwis {
    class CoordinateSystem;
    typedef IlwisData<CoordinateSystem> ICoordinateSystem;
	class RasterCoverage;
	typedef IlwisData<RasterCoverage> IRasterCoverage;
	const QString OSMLAYERNAME = "OSM Layer";

    namespace Ui {

        class BackgroundLayer : public LayerModel {
        public:
            BackgroundLayer();
            BackgroundLayer(LayerManager *manager, QStandardItem *parent, const IOOptions& options);

            Q_INVOKABLE QVector<qreal> vertices(qint32 bufferIndex, const QString &) const;
            Q_INVOKABLE QVector<int> indices(qint32 bufferIndex, const QString &) const;
            Q_INVOKABLE int numberOfBuffers(const QString&) const;
            Q_INVOKABLE QVariant vproperty(const QString& attrName) const;
            Q_INVOKABLE void vproperty(const QString& attrName, const QVariant& value);
            Q_INVOKABLE bool prepare(int);
            virtual bool renderReady();
            virtual void renderReady(bool yesno);
			QString layerData(const Coordinate &crdIn, const QString &attrName, QVariantList &items) const override;

            static LayerModel *create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options);

            NEW_LAYERMODEL(BackgroundLayer);

		public slots:
			void downloadFinished();
			void downloadReadyRead();
			void activateLayer();

        private:
            QColor _backgroundcolor;
            OGLBuffer _buffer;
			IRasterCoverage _osmRaster;
			QNetworkAccessManager _manager;
			QNetworkReply *_currentDownload = nullptr;
			QQueue<QUrl> _downloadQueue;
			QString _osmInputFiles;
			QFile _output;

			BoundingBox _tilesBB;
			int _tailZoom;

			void updateOSMRaster();
			QString makeFileName(const QString& url) const;
			void getData();

        };
    }
}