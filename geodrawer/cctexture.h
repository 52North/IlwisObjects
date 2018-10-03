#pragma once

#include "texture.h"

namespace Ilwis {

	namespace Ui {
	    class CCTexture : public Texture
	    {
	    public:
            CCTexture(RasterLayerModel * rasterLayerModel, const IRasterCoverage & raster, const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, unsigned int zoomFactor);
            virtual ~CCTexture();
            virtual void CreateTexture(bool fInThread, volatile bool * fDrawStop);
            virtual void ReCreateTexture(bool fInThread, volatile bool * fDrawStop);
        protected:
            bool DrawTexture(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, QVector<int> & texture_data, volatile bool* fDrawStop);
	    };
	}
}
