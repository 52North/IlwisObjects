#include <QVector3D>
#include <QtGui/QOpenGLFunctions>
#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "geos/geom/Geometry.h"
#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geometryhelper.h"
#include "clip2tri.h"
#include "ilwistesselator.h"


using namespace Ilwis;
using namespace Ui;


IlwisTesselator::IlwisTesselator()
{
 
 }

IlwisTesselator::~IlwisTesselator()
{
}

void IlwisTesselator::tesselate(const ICoordinateSystem &csyRoot,
                                const ICoordinateSystem &csyGeom,
                                const geos::geom::Geometry *geometry,
								const Coordinate& center,
                                std::vector<qreal>& vertices,
                                std::vector<int> &indices)
{
	std::vector<std::vector<c2t::Point> > holes;
	std::vector<c2t::Point> boundary;
	getContours(geometry, csyRoot, csyGeom, holes, boundary);
	tesselateInternal(holes, boundary, center, vertices, indices);
}

void IlwisTesselator::getContours(const geos::geom::Geometry *geometry,
					const ICoordinateSystem &csyRoot, 
					const ICoordinateSystem &csyGeom, 
					std::vector<std::vector<c2t::Point> >& holes, 
					std::vector<c2t::Point>& boundary)
{
	/*auto incalc = [](const Coordinate& crd1, const Coordinate& crd2)->double {
		return (crd2.x - crd1.x) * (crd2.y + crd1.y);
	};*/
    bool conversionNeeded = csyRoot != csyGeom;
    const geos::geom::Polygon *polygon = dynamic_cast<const geos::geom::Polygon *>(geometry);
    if (polygon){
        auto addCoord = [&](const geos::geom::Coordinate& crd, int i, std::vector<c2t::Point>& contour) {
            if ( conversionNeeded){
                Coordinate crdTransformed = csyRoot->coord2coord(csyGeom,crd);
                contour[i].x = crdTransformed.x;
                contour[i].y = crdTransformed.y;
            } else{
                contour[i].x = crd.x;
                contour[i].y = crd.y;
            }
        };
		Coordinate crdLast;
		const geos::geom::LineString *outerRing = polygon->getExteriorRing();
		boundary.resize(outerRing->getNumPoints() - 1) ;
        for(int i = 0 ; i < boundary.size(); ++i){
            const geos::geom::Coordinate& crd = outerRing->getCoordinateN(i);
		    addCoord(crd, i,boundary);
			crdLast = crd;
        }
        if ( polygon->getNumInteriorRing() > 0){
			holes.resize(polygon->getNumInteriorRing());
            for(int i = 0; i < polygon->getNumInteriorRing(); ++i){
                const geos::geom::LineString *innerRing = polygon->getInteriorRingN(i);
                holes[i].resize(innerRing->getNumPoints() - 1); 
                for(int j = 0 ; j < innerRing->getNumPoints() - 1; ++j){
                    const geos::geom::Coordinate& crd = innerRing->getCoordinateN(j);
					if (crd == crdLast)
						continue;
                    addCoord(crd, j, holes[i]);
                }
            }
        }
    }
}

void IlwisTesselator::tesselateInternal(const std::vector<std::vector<c2t::Point> >& holes, const std::vector<c2t::Point>& boundary, const Coordinate& center, std::vector<qreal>& vertices, std::vector<int> &indices)
{
	vector<c2t::Point> outputPoints, boundingPolygon;  // Every 3 points is a triangle

	c2t::clip2tri clip2tri;
	clip2tri.triangulate(holes, outputPoints, boundary);
	int startIndex = vertices.size();
	vertices.resize(vertices.size() + outputPoints.size() * 3);
	indices.resize(startIndex + outputPoints.size());
	for (int i = 0; i < outputPoints.size(); ++i) {
		c2t::Point& pnt = outputPoints[i];
		vertices[startIndex + i * 3] = pnt.x - center.x;
		vertices[startIndex + i * 3 + 1] = pnt.y - center.y;
		vertices[startIndex + i * 3 + 2] = 0;
		indices[startIndex + i] = startIndex + i;
	}
}
