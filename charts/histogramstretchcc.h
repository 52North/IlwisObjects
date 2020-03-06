#pragma once

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

#include "kernel.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "coverage.h"
#include "chartoperationfactory.h"
#include "chartoperationeditor.h"

namespace Ilwis {
	namespace Ui {
		class LayerModel;
		class ChartModel;
		class VisualAttribute;

		class HistogramStretchCC : public ChartOperationEditor
		{
			Q_OBJECT
		public:
			HistogramStretchCC();

			bool canUse(ChartModel *model, const QVariantMap &parameter) const;

			Q_INVOKABLE void execute(const QVariantMap &parameters);
			static Ilwis::Ui::ChartOperationEditor *create() { return new HistogramStretchCC(); }
			Q_INVOKABLE QObject *view();

			Q_PROPERTY(double min READ min WRITE min NOTIFY rangeChanged)
			Q_PROPERTY(double max READ max WRITE max NOTIFY rangeChanged)
			Q_PROPERTY(double initStretchMin READ initStretchMin CONSTANT)
			Q_PROPERTY(double initStretchMax READ initStretchMax CONSTANT)
			Q_PROPERTY(int resolution READ resolution CONSTANT)

			Q_INVOKABLE void setMarkers(const QVariantList& marks);
			Q_INVOKABLE void setStretchLimit(const QString& attribute, double perc);
			Q_INVOKABLE void markersConfirmed();
			Q_INVOKABLE QString band() const;

			void prepare(ChartModel *model);

			NEW_CHARTPROPERTYEDITOR(HistogramStretchCC)

		signals:
			void rangeChanged();
			void markersChanged();

		private:
			LayerModel *_rasterLayer = 0;
			QString _band;

			double min() const;
			void min(double value);
			double max() const;
			void max(double value) const;
			double initStretchMin() const;
			double initStretchMax() const;
			QVariantList markers() const;
			int resolution() const;
			VisualAttribute * vpmodel() const;
			VisualAttribute * vpmodel();
			ICoverage coverage() const;
		};

	}
}



