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
#include "ilwiscoreui_global.h"
#include "coveragedisplay\coveragelayermodel.h"
#include "location.h"
#include "geos/geom/Coordinate.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "rasterlayermodel.h"

namespace Ilwis {
	class RasterCoverage;
	typedef IlwisData<RasterCoverage> IRasterCoverage;

	namespace Ui {
		class ColorCompositeLayerModel : public RasterLayerModel {

		public:
			ColorCompositeLayerModel();
			ColorCompositeLayerModel(LayerManager *manager, QStandardItem *parent, const QString &name, const QString &desc, const IOOptions& options);

			bool prepare(int prepType);
			virtual QVariant vproperty(const QString& pName) const;
			virtual void vproperty(const QString& pName, const QVariant& value);

			virtual void coverage(const ICoverage& cov);

			virtual QString value2string(const QVariant& value, const QString& attrName = "") const;
			void setActiveAttribute(int idx);
			void fillAttributes();
			IGeoReference georeference() const;
			void linkAcceptMessage(const QVariantMap& parameters) override;
			QVariant coord2value(const Coordinate &c, const QString &attrname) const override;

			static LayerModel *create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options);
		protected:
			virtual void refreshStretch();
		private:
			void init();
			void requestRedraw();
			bool usesColorData() const;
			std::vector<IRasterCoverage> _ccBands;
			std::vector<NumericRange> _currentStretchRanges;
			std::vector <double> _limitMax;
			std::vector<double> _limitMin;
			QColor _selectColor;

			void calcMinMaxSelection(double value, const SPNumericRange& nrng, double& rmin, double& rmax) const;

			NEW_LAYERMODEL(ColorCompositeLayerModel);
		};
	}
}
