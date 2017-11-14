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
