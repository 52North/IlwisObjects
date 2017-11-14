#ifndef DRAWINGCOLOR_H
#define DRAWINGCOLOR_H

namespace Ilwis {
class CoverageLayerModel;

namespace Geodrawer{

class SpatialDataDrawer;

class DrawingColor
{
public:
    DrawingColor(CoverageLayerModel *spatialdrawer);

private:
   CoverageLayerModel *_spatialdrawer;

   QColor clrVal(double rVal) const;
};
}
}

#endif // DRAWINGCOLOR_H
