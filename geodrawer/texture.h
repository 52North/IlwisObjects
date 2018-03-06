#pragma once

#include "raster.h"

namespace Ilwis {

	namespace Ui {
	    class Quad;
        class RasterLayerModel;
	    class Texture
	    {
	    public:
            Texture(RasterLayerModel * rasterLayerModel, const IRasterCoverage & raster, Quad * quad, const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, unsigned int zoomFactor, unsigned int iPaletteSize);
            virtual ~Texture();
            virtual void CreateTexture(bool fInThread, volatile bool * fDrawStop);
            virtual void ReCreateTexture(bool fInThread, volatile bool * fDrawStop);
            bool equals(Quad * quad, const long offsetX1, const long offsetY1, const long offsetX2, const long offsetY2, unsigned int zoomFactor);
            bool contains(const long offsetX1, const long offsetY1, const long offsetX2, const long offsetY2);
            const unsigned int getZoomFactor() const;
            const bool fValid() const;
            void SetDirty();
            const bool fDirty() const;
            const long getOffsetX() const;
            const long getOffsetY() const;
            const unsigned long getSizeX() const;
            const unsigned long getSizeY() const;
            const QVector<int> & data() const;
        protected:
            bool DrawTexture(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, QVector<int> & texture_data, volatile bool* fDrawStop);
            bool DrawTexturePaletted(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, QVector<int> & texture_data, volatile bool* fDrawStop);            
            QVector<int> texture_data;
            const unsigned long sizeX, sizeY;
            const long offsetX, offsetY;
            const unsigned int zoomFactor;
            const unsigned int iPaletteSize;
            const quint64 domain;
            bool valid;
            bool dirty;
            const IRasterCoverage & _raster;
            RasterLayerModel * _rasterLayerModel;
            Quad * _quad;
	    };
	}
}
