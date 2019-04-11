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
        class ChartOperationEditor;

        class ItemCategory {
        public:
            int upper;
            QString label;
        };

        class ILWISCOREUISHARED_EXPORT ChartModel : public QObject
        {
            Q_OBJECT

        public:
            enum class Axis { AXAXIS = 1, AYAXIS = 2, AZAXIS = 3};
            enum class AxisType { AT_VALUE = 1, AT_DATETIME = 2, AT_CATEGORIES = 3};
            enum class NiceFormatting { NF_NONE = 0, NF_XAXIS = 1, NF_YAXIS_LEFT = 2, NF_YAXIS_RIGHT = 3, NF_XY_LEFT = 4, NF_XYRIGHT = 5, NF_XY_ALL = 6};
            Q_ENUM(Axis)
            Q_ENUM(AxisType)
            Q_ENUM(NiceFormatting)

            Q_PROPERTY(QString chartType READ chartType WRITE chartType NOTIFY chartTypeChanged)
            Q_PROPERTY(QString name      READ name                      NOTIFY nameChanged)
            Q_PROPERTY(int seriesCount   READ seriesCount               NOTIFY seriesCountChanged)
            Q_PROPERTY(QQmlListProperty<Ilwis::Ui::DataseriesModel> series READ series            NOTIFY chartModelChanged)
            Q_PROPERTY(QQmlListProperty<Ilwis::Ui::ChartOperationEditor> operations READ operations NOTIFY operationsChanged)
            Q_PROPERTY(bool legendVisible READ legendVisible            WRITE setLegendVisible    NOTIFY legendChanged)
            Q_PROPERTY(QString legendAlignment    READ legendAlignment  WRITE setLegendAlignment  NOTIFY legendChanged)
            Q_PROPERTY(bool updateDataTable READ updateDataTable NOTIFY dataTableChanged)
			Q_PROPERTY(Ilwis::Ui::TableModel * tableModel READ tableModel WRITE tableModel NOTIFY dataTableChanged)

            Q_PROPERTY(quint16 xaxisType   READ xaxisType)
            Q_PROPERTY(double minX         READ minx                    NOTIFY xAxisChanged)
            Q_PROPERTY(double maxX         READ maxx                    NOTIFY xAxisChanged)
            Q_PROPERTY(double minY         READ miny                    NOTIFY yAxisChanged)
            Q_PROPERTY(double maxY         READ maxy                    NOTIFY yAxisChanged)
            Q_PROPERTY(bool fixedY         READ fixedYAxis   WRITE setFixedYAxis   NOTIFY yAxisChanged)
            Q_PROPERTY(bool niceNumbersY   READ niceNumbersY WRITE setNiceNumbersY NOTIFY yAxisChanged)

            Q_PROPERTY(bool updateSeries READ updateSeries NOTIFY updateSeriesChanged)
            Q_PROPERTY(int tickCountX READ tickCountX WRITE tickCountX NOTIFY tickCountXChanged)
            Q_PROPERTY(int tickCountY READ tickCountY WRITE tickCountY NOTIFY tickCountYChanged)
            Q_PROPERTY(QString formatXAxis READ formatXAxis NOTIFY xAxisChanged)
            Q_PROPERTY(QString formatYAxis READ formatYAxis NOTIFY yAxisChanged)

            Q_PROPERTY(bool xAxisVisible   READ xAxisVisble   NOTIFY chartTypeChanged)
            Q_PROPERTY(bool yAxisVisible   READ yAxisVisble   NOTIFY chartTypeChanged)

            Q_INVOKABLE QColor seriesColor(int seriesIndex);
            Q_INVOKABLE QColor seriesColorItem(int seriesIndex, double v);
            Q_INVOKABLE Ilwis::Ui::DataseriesModel* getSeries(int seriesIndex) const;
            Q_INVOKABLE bool addDataTable(const QString& objid);
            Q_INVOKABLE bool addDataTable(const QString & objid, const QString& xcolumn, const QString& ycolumn,QVariantMap extraParams);
            Q_INVOKABLE void assignParent(QObject *parent);
            Q_INVOKABLE quint32 modelId() const;
            Q_INVOKABLE QString dataTableUrl();
			Q_INVOKABLE QString dataTableId() const;
			Q_INVOKABLE void clearChart();

            ChartModel();
            ChartModel(QObject *parent);
            ~ChartModel();

            quint32 createChart(const QString& name, const Ilwis::ITable& tbl, const QString & cType, const QString& xaxis, const QString& yaxis, const QString& zaxis, const QVariantMap& extra);

            QVariantList linkProperties() const;
            QString name() const;
            static QString mainPanelUrl();
            static QString minimalPanelUrl();
            void chartType(const QString& tp);
            QString chartType() const;

            QQmlListProperty<ChartOperationEditor> operations();
            Q_INVOKABLE ChartOperationEditor* operation(quint32 index);

			int seriesCount() const;
			DataseriesModel* getSeriesByName(const QString name) const;
            DataseriesModel* getSeries(const QString& xcolumn, const QString& ycolumn, const QString& zcolumn) const;
            quint32 deleteSerie(const QString& ycolumn, const QString& zcolumn);
			bool isValidSeries(const ITable& inputTable, const QString columnName) const;	// check if column with column name exists
            quint32 insertDataSeries(const ITable& inputTable, quint32 index, const QString& xcolumn, const QString& ycolumn, const QString& zcolumn, const QVariantMap& extra);
            void updateDataSeries(const ITable& inputTable, const QString& xcolumn, const QString& ycolumn, const QString& zcolumn);
            bool updateSeries() const;
            void tickCountX(int tc);
            int tickCountX() const;
            void tickCountY(int tc);
            int tickCountY() const;
            QColor newColor() const;

            quint16 xaxisType() const;

            // value axis
            double minx() const;
            double maxx() const;
            double miny() const;
            double maxy() const;
            void setMinX(double val);
            void setMaxX(double val);
            void setMinY(double val);
            void setMaxY(double val);
            bool fixedYAxis() const;
            void setFixedYAxis(bool fixed);
            bool niceNumbersY() const;
            void setNiceNumbersY(bool nice);
            bool legendVisible() const;
            void setLegendVisible(bool show);
            QString legendAlignment() const;
            void setLegendAlignment(const QString& align);
			void changeDataSeriesName(const QString& oldName, const QString& newName);

            bool xAxisVisble();
            bool yAxisVisble();

        signals:
            void chartTypeChanged();
			void nameChanged();
			void chartModelChanged();
			void seriesCountChanged();
            void operationsChanged();
			void xAxisChanged();
			void yAxisChanged();
            void updateSeriesChanged();
            void tickCountXChanged();
            void tickCountYChanged();
            void legendChanged();
			void dataTableChanged();

        private:
			QQmlListProperty<DataseriesModel> series();
            void fillOperations();

            void initializeDataSeries(DataseriesModel *newseries);
            QString formatXAxis() const;
            QString formatYAxis() const;
            bool axisCompatible(const DataDefinition& inputDef, Axis axis, bool basicCheck=true);

			bool checkRanges(const NumericRange *nrange1, const NumericRange *nrange2) const;
			void tableModel(TableModel *t);
			TableModel* tableModel() const;
			bool updateDataTable() const;
			void fillTableData();

			double _minx = rUNDEF, _maxx = rUNDEF, _miny = rUNDEF, _maxy = rUNDEF;
            bool _fixedY = false;
            bool _fixedX = false;
            bool _niceNumbersY = false;
            bool _legendVisible = true;
            QString _alignment = "top";

            AxisType _xaxisType;
            QString _chartType = sUNDEF;
            int _tickCountX = 5;
            int _tickCountY = 5;
            quint32 _modelId;
            QString _name = sUNDEF;
            ITable _datatable;
			TableModel *_dataTableModel = 0;

			QList<DataseriesModel *> _series;
            QList<ChartOperationEditor *> _operations;

            std::vector<QString> _graphColors = { "crimson", "blueviolet","olive","deeppink","darkblue","darkred","cadetblue","tomato", "darkslateblue","orangered", "dodgerblue","forestgreen","goldenrod",
            "green","indigo","limegreen","orange", "mediumslateblue","orchid","seagreen","sienna","teal" };
        };
    }
}

#endif // CHARTMODEL_H
