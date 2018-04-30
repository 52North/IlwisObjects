#ifndef CHARTMODEL_H
#define CHARTMODEL_H

#include <QObject>
#include <QQmlListProperty>
#include "ilwis.h"
#include "ilwiscoreui_global.h"

#include <QQmlListProperty>
#include <QAbstractTableModel>
#include "ilwisobjectmodel.h"
#include "table.h"

namespace Ilwis {
    namespace Ui {
        class TableModel;

        class ILWISCOREUISHARED_EXPORT ChartModel : public QObject
        {
            Q_OBJECT

        public:
            enum Axis{aXAXIS, aYAXIS, aZAXIS};

            Q_PROPERTY(QString chartType READ chartType WRITE chartType NOTIFY chartTypeChanged)
			Q_PROPERTY(QString name READ name NOTIFY nameChanged)

            ChartModel();
            ChartModel(QObject *parent);

            quint32 createChart(const QString& name, const Ilwis::ITable& tbl, const QString& chartType, quint32 xaxis, quint32 yaxis, quint32 zaxis);

            QVariantList ChartModel::linkProperties() const;
            quint32 ChartModel::modelId() const;
            QString ChartModel::name() const;
            static QString mainPanelUrl();
            void chartType(const QString& tp);
            QString chartType() const;

        signals:
            void chartTypeChanged();
			void nameChanged();

        private:
            QString _chartType = sUNDEF;
            Ilwis::ITable _table;
            quint32 _modelId;
            QString _name = sUNDEF;
        };
    }
}

#endif // CHARTMODEL_H
