#pragma once

#include <QObject>
#include <QVariant>
#include <QColor>
#include "box.h"
#include "ilwisdata.h"
#include "coveragedisplay\layermodel.h"
#include "coveragedisplay\oglbuffer.h"

namespace Ilwis {
    class CoordinateSystem;
    typedef IlwisData<CoordinateSystem> ICoordinateSystem;

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

            static LayerModel *create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options);

            NEW_LAYERMODEL(BackgroundLayer);

        private:
            QColor _backgroundcolor;
            OGLBuffer _buffer;

        };
    }
}