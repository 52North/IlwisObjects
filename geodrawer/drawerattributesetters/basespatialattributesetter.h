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

#ifndef BASESPATIALATTRIBUTESETTER_H
#define BASESPATIALATTRIBUTESETTER_H

#include "coveragedisplay/drawerattributesetter.h"

namespace Ilwis {
namespace Ui{

class LayerManager;

class BaseSpatialAttributeSetter : public DrawerAttributeSetter
{
public:
    BaseSpatialAttributeSetter(const LayerManager *manager);
    ~BaseSpatialAttributeSetter();

    bool isValid() const;
    bool coordinateConversionNeeded() const;

    void sourceCsySystem(const ICoordinateSystem &source);

protected:
    ICoordinateSystem _sourceSystem;
    ICoordinateSystem _targetSystem;
    const LayerManager *_layerManager = 0;
    bool _conversionNeeded = false;

    Envelope _zoomRect;
};
}
}

#endif // BASESPATIALATTRIBUTESETTER_H
