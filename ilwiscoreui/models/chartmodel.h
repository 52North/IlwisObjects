#ifndef CHARTMODEL_H
#define CHARTMODEL_H

#include <QObject>
#include <QList>
#include <QQmlListProperty>
#include <QColor>
#include "ilwis.h"
#include "ilwiscoreui_global.h"

#include <QQmlListProperty>
#include <QAbstractTableModel>
#include "ilwisobjectmodel.h"
#include "table.h"

namespace Ilwis {
    namespace Ui {
        class TableModel;
		class DataseriesModel;

        class ILWISCOREUISHARED_EXPORT ChartModel : public QObject
        {
            Q_OBJECT

        public:
            enum Axis{aXAXIS, aYAXIS, aZAXIS};

            Q_PROPERTY(QString chartType READ chartType WRITE chartType NOTIFY chartTypeChanged)
                Q_PROPERTY(QString name      READ name                      NOTIFY nameChanged)
                Q_PROPERTY(int seriesCount   READ seriesCount               NOTIFY seriesCountChanged)

                Q_PROPERTY(double minX         READ minx                    NOTIFY xAxisChanged)
                Q_PROPERTY(double maxX         READ maxx                    NOTIFY xAxisChanged)
                Q_PROPERTY(double minY         READ miny                    NOTIFY yAxisChanged)
                Q_PROPERTY(double maxY         READ maxy                    NOTIFY yAxisChanged)
                Q_PROPERTY(bool updateSeries READ updateSeries NOTIFY updateSeriesChanged)
                Q_PROPERTY(int tickCountX READ tickCountX WRITE tickCountX NOTIFY tickCountXChanged)
                Q_PROPERTY(int tickCountY READ tickCountY WRITE tickCountY NOTIFY tickCountYChanged)

                Q_INVOKABLE QColor seriesColor(int seriesIndex);
                Q_INVOKABLE QColor seriesColorItem(int seriesIndex, double v);

            ChartModel();
            ChartModel(QObject *parent);

            quint32 createChart(const QString& name, const Ilwis::ITable& tbl, const QString& chartType, quint32 xaxis, quint32 yaxis, quint32 zaxis);

            QVariantList ChartModel::linkProperties() const;
            quint32 ChartModel::modelId() const;
            QString ChartModel::name() const;
            static QString mainPanelUrl();
            void chartType(const QString& tp);
            QString chartType() const;

			int seriesCount() const;
			Q_INVOKABLE Ilwis::Ui::DataseriesModel* getSeries(int seriesIndex) const;
			Ilwis::Ui::DataseriesModel* getSeriesByName(const QString name) const;
            DataseriesModel* ChartModel::getSeries(int xColumnIndex, int yColumnIndex, int zColumnIndex) const;
            quint32 deleteSerie(int xColumnIndex, int yColumnIndex, int zColumnIndex);
			bool isValidSeries(const QString columnName) const;	// check if column with column name exists
			quint32 addDataSeries(quint32 xaxis, quint32 yaxis, quint32 zaxis, const QColor& color);
            quint32 insertDataSeries(quint32 index, quint32 xaxis, quint32 yaxis, quint32 zaxis, const QColor& color);
            void updateDataSeries(int xaxis, int yaxis, int zaxis);
			Ilwis::ITable table() const;
            bool updateSeries() const;
            void tickCountX(int tc);
            int tickCountX() const;
            void tickCountY(int tc);
            int tickCountY() const;
            QColor newColor() const;

        signals:
            void chartTypeChanged();
			void nameChanged();
			void chartModelChanged();
			void seriesCountChanged();
			void xAxisChanged();
			void yAxisChanged();
            void updateSeriesChanged();
            void tickCountXChanged();
            void tickCountYChanged();

        private:
			double minx() {
				return _minx;
			}
			double maxx() {
				return _maxx;
			}
			double miny() {
				return _miny;
			}
			double maxy() {
				return _maxy;
			}

            void initializeDataSeries(DataseriesModel *newseries);

			double _minx = rUNDEF, _maxx = rUNDEF, _miny = rUNDEF, _maxy = rUNDEF;
			QString _chartType = sUNDEF;
            int _tickCountX = 5;
            int _tickCountY = 5;
            Ilwis::ITable _table;
            quint32 _modelId;
            QString _name = sUNDEF;
			QList<DataseriesModel *> _series;
            std::vector<QString> _graphColors = { "crimson", "blueviolet","olive","deeppink","darkblue","darkred","cadetblue","tomato", "darkslateblue","orangered", "dodgerblue","forestgreen","goldenrod",
            "green","indigo","limegreen","orange", "mediumslateblue","orchid","seagreen","sienna","teal" };
        };
    }
}

#endif // CHARTMODEL_H
