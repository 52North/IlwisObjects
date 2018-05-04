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

