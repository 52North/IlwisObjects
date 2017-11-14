#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "uicontextmodel.h"
#include "coveragedisplay/layermanager.h"
#include "basespatialattributesetter.h"    

using namespace Ilwis;
using namespace Ui;   

BaseSpatialAttributeSetter::BaseSpatialAttributeSetter(const LayerManager *manager) : _layerManager(manager) 
{ 
	_targetSystem = _layerManager->rootLayer()->screenCsy();             
 
}   

BaseSpatialAttributeSetter::~BaseSpatialAttributeSetter()                  
{
 
}

void BaseSpatialAttributeSetter::sourceCsySystem(const ICoordinateSystem &source)    
{
    _sourceSystem = source;
    if ( _targetSystem.isValid() && _sourceSystem.isValid()){     
        _conversionNeeded = _sourceSystem != _targetSystem;  
    } 
}

bool BaseSpatialAttributeSetter::isValid() const 
{
    return _layerManager != 0 && _targetSystem.isValid() && _sourceSystem.isValid();
}

bool BaseSpatialAttributeSetter::coordinateConversionNeeded() const 
{
    return _conversionNeeded;
}
