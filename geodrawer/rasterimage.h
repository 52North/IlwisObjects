#pragma once

#include <QObject>
#include "raster.h"

namespace Ilwis {
	class RasterCoverage;
	typedef IlwisData<RasterCoverage> IRasterCoverage;

	namespace Geodrawer {
		class RasterImage : public QObject
		{
			Q_OBJECT

		public:
			Q_INVOKABLE QVariantMap colorTexture(int id);
			Q_INVOKABLE QVariantMap palettedTexture(int id);

			RasterImage(QObject *parent);

		private:
			IRasterCoverage _raster;
		};
	}
}
