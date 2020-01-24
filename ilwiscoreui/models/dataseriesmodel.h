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
#ifndef DataseriesModel_H
#define DataseriesModel_H

#include <QObject>
#include <QColor>
#include <QXYSeries>
#include <QQmlListProperty>
#include "kernel.h"
#include "ilwisdata.h"
#include "ilwisobject.h"
#include "operationmetadata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operation.h"
#include "attributemodel.h"
#include "chartmodel.h"

namespace  Ilwis {
	class Table;
	typedef IlwisData<Table> ITable;

	namespace Ui {
        class ChartOperationEditor;

		class ILWISCOREUISHARED_EXPORT DataseriesModel : public QObject
		{
            Q_OBJECT

                Q_PROPERTY(QString name            READ name                      NOTIFY isNameChanged)
                Q_PROPERTY(QColor color            READ color      WRITE setColor NOTIFY onColorChanged)
                Q_PROPERTY(QString charttype       READ charttype  WRITE setType  NOTIFY onTypeChanged)
                Q_PROPERTY(QVariantList points     READ points                    NOTIFY onPointsChanged)

                Q_PROPERTY(quint16 xAxisType   READ xAxisType)
                Q_PROPERTY(quint16 yAxisType   READ yAxisType)
                Q_PROPERTY(double minX         READ minx)
                Q_PROPERTY(double maxX         READ maxx)
                Q_PROPERTY(double minY         READ miny)
                Q_PROPERTY(double maxY         READ maxy)
                Q_PROPERTY(int resolutionX READ resolutionX CONSTANT)
                Q_PROPERTY(int resolutionY READ resolutionY CONSTANT)
                Q_PROPERTY(int resolutionZ READ resolutionZ CONSTANT)
                Q_PROPERTY(int minID       MEMBER _minID)
                Q_PROPERTY(int maxID       MEMBER _maxID)
				Q_PROPERTY(QString yAxisSide READ yaxisSide CONSTANT)

                Q_PROPERTY(QQmlListProperty<Ilwis::Ui::ChartOperationEditor> operations READ operations NOTIFY operationsChanged)

		public:

			DataseriesModel(ChartModel *chartModel, const QString& xaxis, const QString& yaxis, const QString& zaxis, const QVariantMap& extraParameters);
			bool setData(const ITable& inputTable, const QVariantMap& extra);
			explicit DataseriesModel(const QString name = "");

			void setColor(const QColor color);
            void setType(const QString type);

			QVariantList points() const;
            QString xColumn() const;
            QString yColumn() const;
            QString zColumn() const;

            quint16 xAxisType() const;
            quint16 yAxisType() const; 

			QString yaxisSide() const { return _yaxisSide; }

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

            double resolutionX();
            double resolutionY();
            double resolutionZ();
            DataDefinition datadefinition(ChartModel::Axis axis) const;

            QQmlListProperty<ChartOperationEditor> operations();
            Q_INVOKABLE ChartOperationEditor* operation(quint32 index);
            Q_INVOKABLE QVariantMap categories(QString axis, bool unique);

			void name(const QString& newName);
			void updateEditors(const QVariantMap& parameters);


		signals:
			void isNameChanged();
			void onColorChanged();
            void onTypeChanged();

			void onPointsChanged();
            void operationsChanged();
            void xcategoriesChanged();

		public slots:
			QString name() const;
			QColor color() const;
            QString charttype() const;

		private:
            quint16 axisType(ChartModel::Axis at) const;
            void fillOperations();

            QString _name = sUNDEF;
			QColor _color = QColor();
            QString _type = sUNDEF;
			bool _selected = false;
			quint32 _seriesIndex;
			QString _xaxis=sUNDEF, _yaxis=sUNDEF, _zaxis=sUNDEF;
            DataDefinition _dataDefinitions[3];

            double _minx = rUNDEF, _maxx = rUNDEF, _miny = rUNDEF, _maxy = rUNDEF;
            int _minID, _maxID;
			QVariantList _points;
			QString _yaxisSide = "left";
            QList<ChartOperationEditor *> _operations;
			int _xres = iUNDEF;
			int _yres = iUNDEF;
		};
	}
}


#endif