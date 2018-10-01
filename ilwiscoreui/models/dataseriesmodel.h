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
                Q_PROPERTY(double minX         READ minx)
                Q_PROPERTY(double maxX         READ maxx)
                Q_PROPERTY(double minY         READ miny)
                Q_PROPERTY(double maxY         READ maxy)
                Q_PROPERTY(int resolutionX READ resolutionX CONSTANT)
                Q_PROPERTY(int resolutionY READ resolutionY CONSTANT)
                Q_PROPERTY(int resolutionZ READ resolutionZ CONSTANT)
                Q_PROPERTY(int minID       MEMBER _minID)
                Q_PROPERTY(int maxID       MEMBER _maxID)

                Q_PROPERTY(QQmlListProperty<Ilwis::Ui::ChartOperationEditor> operations READ operations NOTIFY operationsChanged)

		public:

			DataseriesModel(ChartModel *chartModel, const QString& xaxis, const QString& yaxis, const QString& zaxis, const QColor& color);
			bool setData(const ITable& inputTable);
			explicit DataseriesModel(const QString name = "");

			void setColor(const QColor color);
            void setType(const QString type);

			QVariantList points() const;
            QString xColumn() const;
            QString yColumn() const;
            QString zColumn() const;

            quint16 xAxisType() const;

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
            Q_INVOKABLE ChartOperationEditor* operation(quint32 index);
            Q_INVOKABLE QVariantMap categories(QString axis);


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
			QString _name = sUNDEF;
			QColor _color = QColor();
            QString _type = "line";
			bool _selected = false;
			quint32 _seriesIndex;
			QString _xaxis=sUNDEF, _yaxis=sUNDEF, _zaxis=sUNDEF;
            DataDefinition _dataDefinitions[3];

            double _minx = rUNDEF, _maxx = rUNDEF, _miny = rUNDEF, _maxy = rUNDEF;
            int _minID, _maxID;
			QVariantList _points;
            QList<ChartOperationEditor *> _operations;
		};
	}
}


#endif