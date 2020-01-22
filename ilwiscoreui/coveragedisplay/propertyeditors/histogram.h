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



#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/visualpropertyeditor.h"

namespace Ilwis {
	namespace Ui {


		class Histogram : public VisualPropertyEditor
		{
			Q_OBJECT

		public:
			Histogram();
			Histogram(VisualAttribute *p);

			Q_PROPERTY(QString tableUrl READ tableUrlPrivate CONSTANT)
			Q_PROPERTY(int polyCount READ polyCount NOTIFY polyChanged)
			Q_PROPERTY(bool useAOI READ useAOI WRITE useAOI NOTIFY aoiChanged)
			Q_PROPERTY(bool aggregateAOIs READ aggregateAOIs WRITE aggregateAOIs NOTIFY aggregateAOIsChanged)
			

			Q_INVOKABLE void addPoint(int x, int y);
			Q_INVOKABLE QVariantList polygon(int index) const;
			Q_INVOKABLE void addEmptyPolygon();
			Q_INVOKABLE void deleteLastAOI();
			Q_INVOKABLE void deleteAllAOIs();
	
			bool canUse(const IIlwisObject &obj, const DataDefinition &def) const;
			bool canUse(const IIlwisObject &obj, const QString &name) const;
			static VisualPropertyEditor *create(VisualAttribute *p);
			void prepare(const Ilwis::IIlwisObject& bj, const DataDefinition &datadef);


		signals:
			void polyChanged();
			void aoiChanged();
			void aggregateAOIsChanged();
	
		private:
			ITable _histogramData;

			QString tableUrlPrivate();
			int polyCount() const;
			void useAOI(bool yesno);
			bool useAOI() const;
			void calculateLocalizedHistogram(const std::vector<Coordinate>& pol, std::vector<NumericStatistics::HistogramBin>& bins,
											NumericRange& rgnCounts, NumericRange& rngCumulatives) const;
			void bins2table(std::vector<NumericStatistics::HistogramBin>& hist, int columnStart)  const;
			void addColumns(int index);
			void deleteColumns(int index);
			std::vector<NumericStatistics::HistogramBin> initializeBins() const;
			void updateRanges(int columnStart, const NumericRange& rgnCounts, const NumericRange& rngCumulatives);

			bool aggregateAOIs() const;
			void aggregateAOIs(bool yesno);

			std::vector<std::vector<Coordinate>> _polygons;
			IRasterCoverage _raster;
			bool _useAOI = false;
			bool _aggregateAOIs = true;

		};
	}
}


