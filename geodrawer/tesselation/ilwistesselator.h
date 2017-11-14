#ifndef ILWISTESSELATOR_H
#define ILWISTESSELATOR_H

namespace Ilwis {
namespace Ui{

class IlwisTesselator
{
public:
    IlwisTesselator();
    ~IlwisTesselator();

    void tesselate(const ICoordinateSystem &csyRoot, const ICoordinateSystem &csyGeom, const geos::geom::Geometry *geometry, 
		const Coordinate& center,std::vector<qreal>& vertices, std::vector<int> &indices);
private:
	void IlwisTesselator::getContours(const geos::geom::Geometry *geometry, const ICoordinateSystem &csyRoot, const ICoordinateSystem &csyGeom, std::vector<std::vector<c2t::Point> >& holes, std::vector<c2t::Point>& boundary);
	void IlwisTesselator::tesselateInternal(const std::vector<std::vector<c2t::Point> >& holes, const std::vector<c2t::Point>& boundary, const Coordinate& center, std::vector<qreal>& vertices, std::vector<int> &indices);

};
}
}

#endif // ILWISTESSELATOR_H
