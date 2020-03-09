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
		class ChartModel;

		class HistogramSelectionCC : public ChartOperationEditor
		{
			Q_OBJECT
		public:
			HistogramSelectionCC();

			bool canUse(ChartModel *model, const QVariantMap &parameter) const;
			Q_PROPERTY(QString band READ band CONSTANT)

			//Q_PROPERTY(double minRange READ minRange CONSTANT)
			//Q_PROPERTY(double maxRange READ maxRange CONSTANT)
			Q_PROPERTY(double currentMin READ currentMinValue WRITE currentMinValue NOTIFY currentValueChanged)
			Q_PROPERTY(double currentMax READ currentMaxValue WRITE currentMaxValue NOTIFY currentValueChanged)
            Q_PROPERTY(bool useRange READ useRange WRITE useRange NOTIFY userRangeChanged)

			Q_INVOKABLE void execute(const QVariantMap &parameters);
			static Ilwis::Ui::ChartOperationEditor *create() { return new HistogramSelectionCC(); } 
			void updateEditor(const QVariantMap& parameters) override;
			Q_INVOKABLE QObject *view();
			Q_INVOKABLE double bandMinValue(const QString& bandName);
			Q_INVOKABLE double bandMaxValue(const QString& bandName);
			Q_INVOKABLE QString getValues();
			Q_INVOKABLE void markersConfirmed();

			NEW_CHARTPROPERTYEDITOR(HistogramSelectionCC)

		signals:
			void currentValueChanged();
			void currentXChanged();
			void userRangeChanged();
			void rangeChanged();

		private:
			double _minValue = 9999999.0;
			double _maxValue = 9999999.0;
			bool _useRange = false;
			HistogramSelectionCC * _cceditorRed = 0;
			HistogramSelectionCC * _cceditorBlue = 0;
			HistogramSelectionCC * _cceditorGreen = 0;

			QString _band = sUNDEF;

			double currentMinValue() const;
			void currentMinValue(double v);
			double currentMaxValue() const;
			void currentMaxValue(double v);
			bool useRange() const;
			void useRange(bool yesno);

			QString band() const;
			void initBands();

		};

	}
}



