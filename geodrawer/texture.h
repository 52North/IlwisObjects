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

#include "raster.h"

#define TextureData QByteArray 

namespace Ilwis {

	namespace Ui {
        class RasterLayerModel;
	    class Texture
	    {
	    public:
            Texture(RasterLayerModel * rasterLayerModel, const IRasterCoverage & raster, int bIndex, const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, unsigned int zoomFactor, unsigned int iPaletteSize);
            virtual ~Texture();
            virtual void CreateTexture(bool fInThread, volatile bool * fDrawStop);
            virtual void ReCreateTexture(bool fInThread, volatile bool * fDrawStop);
            bool equals(const long offsetX1, const long offsetY1, const long offsetX2, const long offsetY2, unsigned int zoomFactor);
            bool contains(const long offsetX1, const long offsetY1, const long offsetX2, const long offsetY2);
            const unsigned int getZoomFactor() const;
            const bool fValid() const;
            void SetDirty();
            const bool fDirty() const;
            const long getOffsetX() const;
            const long getOffsetY() const;
            const unsigned long getSizeX() const;
            const unsigned long getSizeY() const;
            const TextureData & data() const;
			int bandIndex() const;
        protected:
            bool DrawTexture(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor,TextureData & texture_data, volatile bool* fDrawStop);
            bool DrawTexturePaletted(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, TextureData & texture_data, volatile bool* fDrawStop);
            double getStretchedValue(double value, const NumericRange& actualRange, const NumericRange& stretchRange) const;
            TextureData texture_data;
            const unsigned long sizeX, sizeY;
            const long offsetX, offsetY;
            const unsigned int zoomFactor;
            const unsigned int iPaletteSize;
            const quint64 domain;
            bool valid;
            bool dirty;
            const IRasterCoverage & _raster;
            RasterLayerModel * _rasterLayerModel;
			int _bandIndex = 0;
	    };
	}
}
