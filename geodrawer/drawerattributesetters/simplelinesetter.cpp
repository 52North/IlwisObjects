#include <QVector3D>
#include <QtGui/QOpenGLFunctions>
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "geometryhelper.h"
#include "attributemodel.h"
#include "coveragedisplay/visualattribute.h"
#include "coveragedisplay/drawerattributesetterfactory.h"
#include "coveragedisplay/coveragelayermodel.h"
#include "../featurelayermodel.h"
#include "coveragedisplay/layermanager.h"
#include "simplelinesetter.h"

using namespace Ilwis;
using namespace Ui;

SimpleLineSetter::SimpleLineSetter(const LayerManager *manager) : BaseSpatialAttributeSetter(manager)
{

}

SimpleLineSetter::~SimpleLineSetter()
{

}

void SimpleLineSetter::getGeometryVertices(const geos::geom::Geometry* geom, std::vector<qreal>& vertices, std::vector<int>& indices) const {

	if (!geom)
		return;

	const std::vector<geos::geom::Coordinate>* coords = geom->getCoordinates()->toVector();

	Coordinate crd;
	int extraIndices = (int)coords->size() * 2 - 2;
	int currentIndicesSize = (int)indices.size();
	int verticesIndex = (int)vertices.size() / 3;
	int indicesIndex = currentIndicesSize;
	indices.resize(currentIndicesSize + extraIndices);
	for (int i = 0; i < coords->size(); ++i) {
		if (i > 1)
			indices[indicesIndex++] = verticesIndex + i - 1;

		crd = coords->at(i);
		if (coordinateConversionNeeded()) {
			crd = _targetSystem->coord2coord(_sourceSystem, crd);
		}
		vertices.push_back(crd.x);
		vertices.push_back(crd.y);
		vertices.push_back((std::isnan(crd.z) || crd.z == rUNDEF) ? 0 : crd.z);
		indices[indicesIndex++] = verticesIndex + i;
	}
}

void SimpleLineSetter::getVerticesPolygon(const geos::geom::Geometry *polygons, std::vector<qreal>& vertices, std::vector<int>& indices) const {
	geos::geom::GeometryTypeId geostype = polygons->getGeometryTypeId();
	if (geostype == geos::geom::GEOS_POLYGON) {
		const geos::geom::Polygon *polygon = dynamic_cast<const geos::geom::Polygon*>(polygons);
		getGeometryVertices(polygon->getExteriorRing(), vertices, indices);
		//indices.push_back(startIndex);
		quint32 holeCount = (quint32)polygon->getNumInteriorRing();
		for (quint32 i = 0; i < holeCount; ++i) {
			getGeometryVertices(polygon->getInteriorRingN(i), vertices, indices);
		}
		
	}
	else {
		int n = (quint32)polygons->getNumGeometries();
		for (int i = 0; i < n; ++i) {
			const geos::geom::Polygon* polygon = dynamic_cast<const geos::geom::Polygon*>(polygons->getGeometryN(i));
			getGeometryVertices(polygon->getExteriorRing(), vertices, indices);
            quint32 holeCount = (quint32)polygon->getNumInteriorRing();
            for (quint32 i = 0; i < holeCount; ++i) {
                getGeometryVertices(polygon->getInteriorRingN(i), vertices, indices);
            }
		}
	}
}

void SimpleLineSetter::getVertices(const geos::geom::Geometry *geometry, std::vector<qreal>& vertices, std::vector<int>& indices)const
{
    geos::geom::GeometryTypeId geostype = geometry->getGeometryTypeId();
	if (geostype == geos::geom::GEOS_POLYGON || geostype == geos::geom::GEOS_MULTIPOLYGON) {
		getVerticesPolygon(geometry, vertices, indices);
		return;
	}
    int n = (int)geometry->getNumGeometries();
    for(int  geom = 0; geom < n; ++geom ){
        const geos::geom::Geometry *subgeom = geometry->getGeometryN(geom);
        if (!subgeom)
            continue;
		getGeometryVertices(subgeom, vertices, indices);
     }
}

void SimpleLineSetter::getColors(const VisualAttribute &attr, const QVariant &value, const QColor &defaultColor, int start, std::vector<qreal>& colors) const
{
	QColor clr = attr.value2color(value);
	for (int j = start; j < colors.size(); j += 3) {
		if (value.isValid() && value.toInt() != iUNDEF) {
				colors[j] = clr.redF();
				colors[j + 1] = clr.greenF();
				colors[j + 2] = clr.blueF();
		}
	}
}

