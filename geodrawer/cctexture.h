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

#include "texture.h"

namespace Ilwis {

	namespace Ui {
	    class CCTexture : public Texture
	    {
	    public:
            CCTexture(RasterLayerModel * rasterLayerModel, const std::vector<IRasterCoverage> ccBands, const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, unsigned int zoomFactor);
            virtual ~CCTexture();
            virtual void CreateTexture(bool fInThread, volatile bool * fDrawStop);
            virtual void ReCreateTexture(bool fInThread, volatile bool * fDrawStop);
        protected:
            bool DrawTexture(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, QVector<int> & texture_data, volatile bool* fDrawStop);
			std::vector<IRasterCoverage> _ccBands;
	    };
	}
}
