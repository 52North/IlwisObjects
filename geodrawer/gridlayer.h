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

#ifndef GRIDDRAWER_H
#define GRIDDRAWER_H

#include <QObject>
#include <QVariant>
#include <QColor>
#include "box.h"
#include "ilwisdata.h"
#include "coveragedisplay\layermodel.h"
#include "coveragedisplay\oglbuffer.h"
#include "coveragedisplay\compositelayermodel.h"

namespace Ilwis {
    class CoordinateSystem;
    typedef IlwisData<CoordinateSystem> ICoordinateSystem;

    namespace Ui {

        class GridLayer : public CompositeLayerModel {
        public:
            GridLayer();
            GridLayer(LayerManager *manager, QStandardItem *parent, const IOOptions& options);

            bool prepare(int);
            QVariant vproperty(const QString& attrName) const;
            void vproperty(const QString& attrName, const QVariant& value);
            static LayerModel *create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options);

            NEW_LAYERMODEL(GridLayer);

        };

        class SubGridLayer : public LayerModel
        {
        public:
            SubGridLayer(LayerManager *manager, QStandardItem *parent, const QString &name, const QString &desc, const IOOptions& options);

            Q_INVOKABLE QVector<qreal> vertices(qint32 bufferIndex, const QString &) const;
            Q_INVOKABLE QVector<int> indices(qint32 bufferIndex, const QString &) const;
            Q_INVOKABLE int numberOfBuffers(const QString&) const;
            Q_INVOKABLE QVariant vproperty(const QString& attrName) const;
            Q_INVOKABLE void vproperty(const QString& attrName, const QVariant& value);
            void setSecondaryGrid(SubGridLayer *sec);
            virtual bool renderReady();
            virtual void renderReady(bool yesno);


        protected:
            void calcEnvelope(Coordinate &cmin, Coordinate &cmax) const;

            OGLBuffer _buffer;
            double _cellDistance = rUNDEF;
            QColor _lineColor;
            float _linewidth = 1.0;
            ICoordinateSystem _latlon;
            SubGridLayer *_secondaryGrid=0;


        };

        class PrimaryGridLayer : public SubGridLayer {
        public:
            PrimaryGridLayer(LayerManager *manager, QStandardItem *parent, const IOOptions& options);
            bool prepare(int dummy);
            void active(bool yesno);
            bool active() const;
               
         };

        class SecondaryGridLayer : public SubGridLayer {
        public:
            SecondaryGridLayer(LayerManager *manager, QStandardItem *parent, PrimaryGridLayer *primaryGrid, const IOOptions& options);

            Q_INVOKABLE QVariant vproperty(const QString& attrName) const;
            Q_INVOKABLE void vproperty(const QString& attrName, const QVariant& value);
            bool prepare(int dummy);
            void active(bool yesno);

        private:
            int _cellCount = 4;
            PrimaryGridLayer *_primaryGrid;


        };
    }
}

#endif // GRIDDRAWER_H

