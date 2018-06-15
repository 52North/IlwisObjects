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

			Q_PROPERTY(QString name        READ name                 NOTIFY isNameChanged)
			Q_PROPERTY(QColor color        READ color WRITE setColor NOTIFY onColorChanged)
			Q_PROPERTY(QVariantList points READ points               NOTIFY onPointsChanged)

			Q_PROPERTY(double minX         READ minx)
			Q_PROPERTY(double maxX         READ maxx)
			Q_PROPERTY(double minY         READ miny)
			Q_PROPERTY(double maxY         READ maxy)
            Q_PROPERTY(int resolutionX READ resolutionX CONSTANT)
            Q_PROPERTY(int resolutionY READ resolutionY CONSTANT)
            Q_PROPERTY(int resolutionZ READ resolutionZ CONSTANT)
            Q_PROPERTY(QQmlListProperty<Ilwis::Ui::ChartOperationEditor> operations READ operations NOTIFY operationsChanged)

		public:

			DataseriesModel(ChartModel *chartModel, const QString& xaxis, const QString& yaxis, const QString& zaxis, const QColor& color);
			bool setData(const ITable& inputTable);
			explicit DataseriesModel(const QString name = "");

			void setColor(const QColor color);

			QVariantList points() const;
            QString xColumn() const;
            QString yColumn() const;
            QString zColumn() const;

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
            DataDefinition datadefinition(ChartModel::Axis axis);

            QQmlListProperty<ChartOperationEditor> operations();
            Q_INVOKABLE Ilwis::Ui::ChartOperationEditor* operation(quint32 index);


		signals:
			void isNameChanged();
			void onColorChanged();
			void onPointsChanged();
            void operationsChanged();

		public slots:
			QString name() const;
			QColor color() const;

		private:
			QString _name = sUNDEF;
			QColor _color = QColor();
			bool _selected = false;
			quint32 _seriesIndex;
			QString _xaxis=sUNDEF, _yaxis=sUNDEF, _zaxis=sUNDEF;
            DataDefinition _dataDefinitions[3];
			double _minx = rUNDEF, _maxx = rUNDEF, _miny = rUNDEF, _maxy = rUNDEF;
			QVariantList _points;
            QList<ChartOperationEditor *> _operations;
		};
	}
}


#endif