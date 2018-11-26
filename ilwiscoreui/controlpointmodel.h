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
#include <QColor>
#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "controlpoint.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
    namespace Ui {
        class LayerManager;

        class ILWISCOREUISHARED_EXPORT ControlPointModel : public QObject
        {
            Q_OBJECT

                friend class ControlPointsListModel;

                Q_PROPERTY(double x READ x WRITE x NOTIFY updateControlPoint)
                Q_PROPERTY(double y READ y WRITE y NOTIFY updateControlPoint)
                Q_PROPERTY(double column READ column WRITE column NOTIFY updateControlPoint)
                Q_PROPERTY(double row READ row WRITE row NOTIFY updateControlPoint)
                Q_PROPERTY(int columnScreen READ columnScreen NOTIFY updateControlPoint)
                Q_PROPERTY(int rowScreen READ rowScreen NOTIFY updateControlPoint)
                Q_PROPERTY(bool active READ active WRITE active NOTIFY activeChanged)
                Q_PROPERTY(double columnError READ columnError WRITE columnError NOTIFY updateControlPoint)
                Q_PROPERTY(double rowError READ rowError WRITE rowError NOTIFY updateControlPoint)
                Q_PROPERTY(QString label READ label WRITE label NOTIFY updateControlPoint)
                Q_PROPERTY(QString color READ color NOTIFY updateControlPoint)
        public:
            ControlPointModel(QObject *parent=0);
            ControlPointModel(LayerManager *manager, const QString lab, bool active, double x, double y, quint32 column, quint32 row, double columnError, double rowError,  QObject *parent);
            ~ControlPointModel();

            double x() const;
            void x(double value);
            double y() const;
            void y(double value);
            double row() const;
            void row(double r);
            double column() const;
            void column(double c);
            double rowError() const;
            void rowError(double value);
            double columnError() const;
            void columnError(double value);
            bool active() const;
            void active(bool yesno);
            ControlPoint controlPoint();
            void update();
            int rowScreen() const;
            int columnScreen() const;
            void screenPosition(double c, double r);
            QString label() const;
            void label(const QString& l);
            QString color() const;
            void color(const QColor& clr);
            bool isValid() const;

        signals:
            void updateControlPoint();
            void activeChanged();

        private:
            ControlPoint _controlPoint;
            Coordinate _screenCrd;
            LayerManager *_layerManager = 0;
            QString _label;
            QColor _color = QColor("red");

        };
    }
}
