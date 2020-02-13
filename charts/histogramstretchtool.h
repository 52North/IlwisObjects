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
#include "chartoperationfactory.h"
#include "chartoperationeditor.h"

namespace Ilwis {
	namespace Ui {
		class LayerModel;
		class ChartModel;

		class HistogramStretchTool : public ChartOperationEditor
		{
			Q_OBJECT
		public:
			HistogramStretchTool();

			bool canUse(ChartModel *model, const QVariantMap &parameter) const;

			Q_INVOKABLE void execute(const QVariantMap &parameters);
			Q_PROPERTY(double actualMin READ actualMin CONSTANT)
			Q_PROPERTY(double actualMax READ actualMax CONSTANT)
			Q_PROPERTY(QVariantMap markers READ markers NOTIFY onMarkersChanged)

			Q_INVOKABLE void addIntensityCurve();
			Q_INVOKABLE void removeIntensityCurve();
			Q_INVOKABLE void stretchHistogram(bool isLower, double origValue, double newValue);
			Q_INVOKABLE void updateTextures();

			void prepare(ChartModel *model);
			static Ilwis::Ui::ChartOperationEditor *create() { return new HistogramStretchTool(); }

			NEW_CHARTPROPERTYEDITOR(HistogramStretchTool)

		signals:
			void currentValueChanged();
			void onMarkersChanged();


		private:
			double _value;
			LayerModel *_rasterLayer;
			NumericRange _actualRange;
			QVector<QPointF> _basePoints; //  the points before we had any strecth
			std::vector<std::pair<double, double>> _mapping;

			double currentValue() const;
			double actualMin() const;
			double actualMax() const;
			QVariantMap markers() const;

		};

	}
}



