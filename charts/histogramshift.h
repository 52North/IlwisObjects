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

		class HistogramShift : public ChartOperationEditor
		{
			Q_OBJECT
		public:
			HistogramShift();

			Q_PROPERTY(double minValue READ minValue NOTIFY minValueChanged)
			Q_PROPERTY(double maxValue READ maxValue NOTIFY maxValueChanged)
			Q_PROPERTY(bool isActive READ isActive NOTIFY isActiveChanged)


			bool canUse(ChartModel *model, const QVariantMap &parameter) const;

			Q_INVOKABLE void execute(const QVariantMap &parameters);
			Q_INVOKABLE void addIntensityCurve();
			Q_INVOKABLE void removeIntensityCurve();
			Q_INVOKABLE void shiftValues(double shift);

			static Ilwis::Ui::ChartOperationEditor *create() { return new HistogramShift(); }

			NEW_CHARTPROPERTYEDITOR(HistogramShift)

		signals:
			void minValueChanged();
			void maxValueChanged();
			void isActiveChanged();

		private:
			double minValue() const;
			double maxValue() const;
			bool isActive() const;
			QVector<QPointF> _basePoints; //  the points before we had any shift


		};

	}
}



