#pragma once

#include <QObject>
#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "controlpoint.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
    namespace Ui {

        class ILWISCOREUISHARED_EXPORT ControlPointModel : public QObject
        {
            Q_OBJECT

            Q_PROPERTY(double x READ x WRITE x NOTIFY updateControlPoint)
            Q_PROPERTY(double y READ y WRITE y NOTIFY updateControlPoint)
            Q_PROPERTY(double column READ column WRITE column NOTIFY updateControlPoint)
            Q_PROPERTY(double row READ row WRITE row NOTIFY updateControlPoint)
            Q_PROPERTY(int columnScreen READ columnScreen WRITE columnScreen NOTIFY updateControlPoint)
            Q_PROPERTY(int rowScreen READ rowScreen WRITE rowScreen NOTIFY updateControlPoint)
            Q_PROPERTY(bool active READ active WRITE active NOTIFY activeChanged)
            Q_PROPERTY(double columnError READ columnError WRITE columnError NOTIFY updateControlPoint)
            Q_PROPERTY(double rowError READ rowError WRITE rowError NOTIFY updateControlPoint)
        public:
            ControlPointModel(QObject *parent=0);
            ControlPointModel(bool active, double x, double y, quint32 column, quint32 row, double columnError, double rowError,  QObject *parent);
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
            void rowScreen(double r);
            int columnScreen() const;
            void columnScreen(double c);

        signals:
            void updateControlPoint();
            void activeChanged();

        private:
            ControlPoint _controlPoint;
            double _screenColumn;
            double _screenRow;

        };
    }
}
