#pragma once

#include <QObject>
#include <QQmlListProperty>
#include "ilwisdata.h"
#include "geometries.h"
#include "controlpoint.h"
#include "controlpointmodel.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
    namespace Ui {
        class ILWISCOREUISHARED_EXPORT ControlPointsListModel : public QObject
        {
            Q_OBJECT

            Q_PROPERTY(QQmlListProperty<Ilwis::Ui::ControlPointModel> controlPoints READ controlPoints NOTIFY controlPointsChanged)

        public:
            ControlPointsListModel(QObject *parent=0);
            ~ControlPointsListModel();

            Q_INVOKABLE void addTiepoint();

            signals:
                void controlPointsChanged();

        private:
            QList<Ilwis::Ui::ControlPointModel *> _controlPoints;

            QQmlListProperty<ControlPointModel> controlPoints();
        };
    }
}
