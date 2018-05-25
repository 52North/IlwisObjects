#pragma once
#ifndef DataseriesModel_H
#define DataseriesModel_H

#include <QObject>
#include <QColor>
#include <QXYSeries>
#include <QQmlListProperty>
#include "kernel.h"
#include "ilwisdata.h"
#include "attributemodel.h"
#include "chartmodel.h"

namespace  Ilwis {
	class Table;
	typedef IlwisData<Table> ITable;

	namespace Ui {

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

		public:
			explicit DataseriesModel();
			DataseriesModel(ChartModel *chartModel, quint32 x_index, quint32 y_index, quint32 z_axis);
			bool setData();

			QVariantList points() const;
            int xColumnIndex() const;
            int yColumnIndex() const;
            int zColumnIndex() const;

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

		signals:
			void isNameChanged();
			void onColorChanged();
			void onPointsChanged();

		public slots:
			QString name() const;
			QColor color() const;
			void setColor(const QColor color);

		private:
			QString _name = sUNDEF;
			QColor _color = QColor();
			bool _selected = false;
			quint32 _columnIndex;
			quint32 _xaxis, _yaxis, _zaxis;
			double _minx = rUNDEF, _maxx = rUNDEF, _miny = rUNDEF, _maxy = rUNDEF;
			QVariantList _points;
			Ilwis::ITable _table;

		};
	}
}


#endif