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

			static LayerModel *create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options);
		protected:
			virtual void refreshStretch();
		private:
			void init();
			void requestRedraw();
			bool usesColorData() const;
			std::vector<IRasterCoverage> _ccBands;
			std::vector<NumericRange> _currentStretchRanges;

			NEW_LAYERMODEL(ColorCompositeLayerModel);
		};
	}
}
