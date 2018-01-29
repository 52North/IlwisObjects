#ifndef ILWISTESSELATOR_H
#define ILWISTESSELATOR_H

#include "tesselator.h"

namespace Ilwis {
namespace Ui{

class IlwisTesselator
{
public:
    IlwisTesselator();
    ~IlwisTesselator();

    void tesselate(const ICoordinateSystem &csyRoot, const ICoordinateSystem &csyGeom, const geos::geom::Geometry *geometry, 
        const Coordinate& center, std::vector<qreal>& vertices, std::vector<int> &indices);
private:
   std::vector<std::vector<float>> getContours(const geos::geom::Geometry *geometry,const ICoordinateSystem &csyRoot, const ICoordinateSystem &csyGeom);
   void tesselateInternal(const std::vector<std::vector<float> > &contours, const Coordinate& center, std::vector<qreal>& vertices, std::vector<int> &indices);

    TESStesselator *_tessaltor = 0;
    TESSalloc _ma;
    int _allocated = 0;
};
}
}

#endif // ILWISTESSELATOR_H
