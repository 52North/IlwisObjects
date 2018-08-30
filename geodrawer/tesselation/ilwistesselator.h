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

    void tesselate(const geos::geom::Geometry *geometry, std::vector<qreal>& vertices, std::vector<int> &indices);
private:
   std::vector<std::vector<float>> getContours(const geos::geom::Geometry *geometry);
   void tesselateInternal(const std::vector<std::vector<float> > &contours, std::vector<qreal>& vertices, std::vector<int> &indices);

    TESStesselator *_tessaltor = 0;
    TESSalloc _ma;
    int _allocated = 0;
};
}
}

#endif // ILWISTESSELATOR_H
