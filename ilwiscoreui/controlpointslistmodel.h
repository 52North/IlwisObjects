#pragma once

#include <QObject>
#include <QQmlListProperty>
#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "raster.h"
#include "controlpoint.h"
#include "controlpointmodel.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
    class PlanarCTPGeoReference;

    namespace Ui {
        class LayerManager;
        class ILWISCOREUISHARED_EXPORT ControlPointsListModel : public QObject
        {
            Q_OBJECT

            Q_PROPERTY(QQmlListProperty<Ilwis::Ui::ControlPointModel> controlPoints READ controlPoints NOTIFY controlPointsChanged)
            Q_PROPERTY(QString errors READ errors NOTIFY errorChanged)
            Q_PROPERTY(QString transformation READ transformation WRITE transformation NOTIFY transformationChanged)
            Q_PROPERTY(QString associatedUrl READ associatedUrl CONSTANT)
            Q_PROPERTY(bool postDrawerActive READ postDrawerActive WRITE postDrawerActive NOTIFY postDrawerActiveChanged)
            Q_PROPERTY(int selectedRow READ selectedRow WRITE selectedRow NOTIFY selectedRowChanged)
            Q_PROPERTY(int decimalsCrds READ decimalsCrds CONSTANT)
            Q_PROPERTY(bool subPixelPrecision READ subPixelPrecision WRITE subPixelPrecision NOTIFY subPixePrecisionChanged)
            Q_PROPERTY(QString georefid READ georefid CONSTANT)

        public:
            ControlPointsListModel(QObject *parent=0);
            ~ControlPointsListModel();

            Q_INVOKABLE void addTiepoint();
            Q_INVOKABLE void removeTiepoint(int idnex);
            Q_INVOKABLE void changeTiePointCoord(int index, double x, double y);
            Q_INVOKABLE void changeTiePointPixel(int index, double x, double y, bool editFromTable);
            Q_INVOKABLE int tiePointRow(int index) const;
            Q_INVOKABLE int tiePointColumn(int index) const;
            Q_INVOKABLE double tiePointX(int index) const;
            Q_INVOKABLE double tiePointY(int index) const;
            Q_INVOKABLE void associatedBackgroundMap(Ilwis::Ui::LayerManager *lm, const QString& objid);
            Q_INVOKABLE void linkModels(Ilwis::Ui::LayerManager *lm);
            Q_INVOKABLE void setCoordinateSystem(const QString& id);
            Q_INVOKABLE QString controlPointLabel(int index);
            Q_INVOKABLE void controlPointLabel(int index, const QString& newLabel);

            Q_INVOKABLE QString associatedUrl() const;
            bool postDrawerActive() const;
            void postDrawerActive(bool yesno);
            void subPixelPrecision(bool yesno);
            bool subPixelPrecision() const;

            signals:
                void controlPointsChanged();
                void errorChanged();
                void transformationChanged();
                void postDrawerActiveChanged();
                void selectedRowChanged();
                void subPixePrecisionChanged();

        public slots:
              void linkAcceptMessage(const QVariantMap& parameters);

        private:
            QList<Ilwis::Ui::ControlPointModel *> _controlPoints;
            QString _lastError;
            IGeoReference _georef;
            PlanarCTPGeoReference *_planarCTP = 0;
            QString _associatedUrl = sUNDEF;
            bool _postDrawerActive = false;
            Ilwis::Ui::LayerManager *_associatedBackgroundMap = 0;
            IRasterCoverage _backgroundRaster;
            int _selectedRow = -1;
            const int ILLEGALVALUE = -1000000;

            QQmlListProperty<ControlPointModel> controlPoints();
            QString errors() const;
            QString transformation() const;
            void transformation(const QString& trans);
            void handleComputeResult(int res);
            void selectedRow(int r);
            int selectedRow() const;
            int decimalsCrds() const;
            void urlCsy(const QString& csy);
            QString urlCsy() const;
            QString georefid() const;

        };
    }
}
