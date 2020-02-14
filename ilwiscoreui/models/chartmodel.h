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
				Q_PROPERTY(double minYLeft         READ minyLeft                    NOTIFY yAxisLeftChanged)
				Q_PROPERTY(double maxYLeft         READ maxyLeft                    NOTIFY yAxisLeftChanged)
				Q_PROPERTY(double minYRight         READ minyRight                    NOTIFY yAxisRightChanged)
				Q_PROPERTY(double maxYRight         READ maxyRight                    NOTIFY yAxisRightChanged)
				Q_PROPERTY(bool fixedYLeft         READ fixedYAxisLeft   WRITE setFixedYAxisLeft   NOTIFY yAxisLeftChanged)
				Q_PROPERTY(bool fixedYRight         READ fixedYAxisRight   WRITE setFixedYAxisRight   NOTIFY yAxisRightChanged)
				Q_PROPERTY(bool niceNumbersYLeft   READ niceNumbersYLeft WRITE setNiceNumbersYLeft NOTIFY yAxisLeftChanged)
				Q_PROPERTY(bool niceNumbersYRight   READ niceNumbersYRight WRITE setNiceNumbersYRight NOTIFY yAxisRightChanged)

				Q_PROPERTY(bool updateSeries READ updateSeries NOTIFY updateSeriesChanged)
				Q_PROPERTY(int tickCountX READ tickCountX WRITE tickCountX NOTIFY tickCountXChanged)
				Q_PROPERTY(int tickCountYLeft READ tickCountYLeft WRITE tickCountYLeft NOTIFY tickCountYLeftChanged)
				Q_PROPERTY(int tickCountYRight READ tickCountYRight WRITE tickCountYRight NOTIFY tickCountYRightChanged)
				Q_PROPERTY(QString formatXAxis READ formatXAxis NOTIFY xAxisChanged)
				Q_PROPERTY(QString formatYAxisLeft READ formatYAxisLeft NOTIFY yAxisLeftChanged)
				Q_PROPERTY(QString formatYAxisRight READ formatYAxisRight NOTIFY yAxisRightChanged)


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
			Q_INVOKABLE void sendOverLink(const QVariantMap& parms) ;
			Q_INVOKABLE void setView(QObject *obj);
			Q_INVOKABLE QObject *view();
			Q_INVOKABLE void updateEditors(const QVariantMap& parameters);


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
			QString specialType() const;

            QQmlListProperty<ChartOperationEditor> operations();
            Q_INVOKABLE ChartOperationEditor* operation(quint32 index);

			int seriesCount() const;
			DataseriesModel* getSeriesByName(const QString name) const;
            DataseriesModel* getSeries(const QString& xcolumn, const QString& ycolumn, const QString& zcolumn) const;
            quint32 deleteSerie(const QString& ycolumn, const QString& zcolumn=sUNDEF);
			quint32 deleteSerieByName(const QString& name);
			bool isValidSeries(const ITable& inputTable, const QString columnName) const;	// check if column with column name exists
            quint32 insertDataSeries(const ITable& inputTable, quint32 index, const QString& xcolumn, const QString& ycolumn, const QString& zcolumn, const QVariantMap& extra);
			void fillTableData();
            void updateDataSeries(const ITable& inputTable, const QString& xcolumn, const QString& ycolumn, const QString& zcolumn);
            bool updateSeries() const;
            void tickCountX(int tc);
            int tickCountX() const;
            void tickCountYLeft(int tc);
            int tickCountYLeft() const;
			void tickCountYRight(int tc);
			int tickCountYRight() const;
            QColor newColor() const;

            quint16 xaxisType() const;

            // value axis
            double minx() const;
            double maxx() const;
            double minyLeft() const;
            double maxyLeft() const;
			double minyRight() const;
			double maxyRight() const;
            void setMinX(double val);
            void setMaxX(double val);
            void setMinYLeft(double val);
            void setMaxYLeft(double val);
			void setMinYRight(double val);
			void setMaxYRight(double val);
            bool fixedYAxisLeft() const;
            void setFixedYAxisLeft(bool fixed);
			bool fixedYAxisRight() const;
			void setFixedYAxisRight(bool fixed);
            bool niceNumbersYLeft() const;
            void setNiceNumbersYLeft(bool nice);
			bool niceNumbersYRight() const;
			void setNiceNumbersYRight(bool nice);
            bool legendVisible() const;
            void setLegendVisible(bool show);
            QString legendAlignment() const;
            void setLegendAlignment(const QString& align);
			void changeDataSeriesName(const QString& oldName, const QString& newName);

            bool xAxisVisble();
            bool yAxisVisble();
			ITable dataTable() const;
			quint32 linkedModelId() const;

        signals:
            void chartTypeChanged();
			void nameChanged();
			void chartModelChanged();
			void seriesCountChanged();
            void operationsChanged();
			void xAxisChanged();
			void yAxisLeftChanged();
			void yAxisRightChanged();
            void updateSeriesChanged();
            void tickCountXChanged();
			void tickCountYLeftChanged();
			void tickCountYRightChanged();
            void legendChanged();
			void dataTableChanged();
			void linkSendMessage(const QVariantMap& parameters);

        private:
			QQmlListProperty<DataseriesModel> series();
            void fillOperations();

            void initializeDataSeries(DataseriesModel *newseries);
            QString formatXAxis() const;
            QString formatYAxisLeft() const;
			QString formatYAxisRight() const;
            bool axisCompatible(const DataDefinition& inputDef, Axis axis, bool basicCheck=true);

			bool checkRanges(const NumericRange *nrange1, const NumericRange *nrange2) const;
			void tableModel(TableModel *t);
			TableModel* tableModel() const;
			bool updateDataTable() const;


			double _minx = rUNDEF, _maxx = rUNDEF, _minyLeft = rUNDEF, _maxyLeft = rUNDEF, _minyRight = rUNDEF, _maxyRight = rUNDEF;
            bool _fixedYLeft = false;
			bool _fixedYRight = false;
            bool _fixedX = false;
            bool _niceNumbersYLeft = false;
			bool _niceNumbersYRight = false;
            bool _legendVisible = true;
            QString _alignment = "top";

            AxisType _xaxisType;
            QString _chartType = sUNDEF;
            int _tickCountX = 5;
            int _tickCountYLeft = 5;
			int _tickCountYRight = 5;
            quint32 _modelId;
            QString _name = sUNDEF;
            ITable _datatable;
			TableModel *_dataTableModel = 0;
			QString _specialType = sUNDEF;
			quint32 _linkedModelId=iUNDEF;

			QList<DataseriesModel *> _series;
            QList<ChartOperationEditor *> _operations;
			QObject *_chartView;

            std::vector<QString> _graphColors = { "red", "blue","green","magneta","orange","darkblue","sienna","yellowgreen", "darkslateblue","orangered", "limegreen","Indigo","goldenrod",
            "dimgray","crimson","chocolate","cyan", "mediumslateblue","orchid","seagreen","darkorchid","teal" };
        };
    }
}

#endif // CHARTMODEL_H
