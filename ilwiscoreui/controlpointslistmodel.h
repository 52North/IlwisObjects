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
			Q_PROPERTY(QString slave READ slaveRaster NOTIFY slaveChanged)

        public:
            ControlPointsListModel(QObject *parent=0);
			ControlPointsListModel(const QVariantMap& parms, QObject *parent);
            ~ControlPointsListModel();

            Q_INVOKABLE void addTiepoint();
            Q_INVOKABLE void removeTiepoint(int idnex);
            Q_INVOKABLE void changeTiePointCoord(int index, double x, double y);
            Q_INVOKABLE void changeTiePointPixel(int index, double x, double y, bool editFromTable);
            Q_INVOKABLE int tiePointRow(int index) const;
            Q_INVOKABLE int tiePointColumn(int index) const;
            Q_INVOKABLE double tiePointX(int index) const;
            Q_INVOKABLE double tiePointY(int index) const;
            Q_INVOKABLE void slaveLayerManager(Ilwis::Ui::LayerManager *lm, const QString& objid);
            Q_INVOKABLE void linkModels(Ilwis::Ui::LayerManager *lm);
            Q_INVOKABLE void setCoordinateSystem(const QString& id);
            Q_INVOKABLE QString controlPointLabel(int index);
            Q_INVOKABLE void controlPointLabel(int index, const QString& newLabel);
			Q_INVOKABLE QString slaveid() const;
            Q_INVOKABLE QString associatedUrl() const;
            bool postDrawerActive() const;
            void postDrawerActive(bool yesno);
            void subPixelPrecision(bool yesno);
            bool subPixelPrecision() const;
			QString slaveRaster() const;

            signals:
                void controlPointsChanged();
                void errorChanged();
                void transformationChanged();
                void postDrawerActiveChanged();
                void selectedRowChanged();
                void subPixePrecisionChanged();
				void slaveChanged();

        public slots:
              void linkAcceptMessage(const QVariantMap& parameters);
              void removeBackgroundLayer();

        private:
            QList<Ilwis::Ui::ControlPointModel *> _controlPoints;
            QString _lastError;
            IGeoReference _georef;
            PlanarCTPGeoReference *_planarCTP = 0;
            QString _associatedUrl = sUNDEF;
            bool _postDrawerActive = false;
            Ilwis::Ui::LayerManager *_slaveLayerManager = 0;
            IRasterCoverage _slaveRaster;
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
