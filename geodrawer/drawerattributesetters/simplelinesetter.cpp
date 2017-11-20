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

void SimpleLineSetter::getGeometryVertices(const geos::geom::Geometry* geom, const Coordinate& pnt, std::vector<qreal>& vertices, std::vector<int>& indices) const {

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
		vertices.push_back(crd.x - pnt.x);
		vertices.push_back(crd.y - pnt.y);
		vertices.push_back((std::isnan(crd.z) || crd.z == rUNDEF) ? 0 : crd.z);
		indices[indicesIndex++] = verticesIndex + i;
	}
}

void SimpleLineSetter::getVerticesPolygon(const SPFeatureI& feature, const Coordinate& pnt, std::vector<qreal>& vertices, std::vector<int>& indices) const {
	const UPGeometry& geom = feature->geometry();
	geos::geom::GeometryTypeId geostype = geom->getGeometryTypeId();
	const geos::geom::Geometry *polygons = geom.get();
	if (geostype == geos::geom::GEOS_POLYGON) {
		const geos::geom::Polygon *polygon = dynamic_cast<const geos::geom::Polygon*>(polygons);
		int startIndex = (int)indices.size();
		getGeometryVertices(polygon->getExteriorRing(), pnt, vertices, indices);
		//indices.push_back(startIndex);
		quint32 holeCount = (quint32)polygon->getNumInteriorRing();
		for (quint32 i = 0; i < holeCount; ++i) {
			startIndex = (int)indices.size();
			getGeometryVertices(polygon->getInteriorRingN(i), pnt, vertices, indices);
			//indices.push_back(startIndex);
		}
		
	}
	else {
		int n = (quint32)polygons->getNumGeometries();
		for (int i = 0; i < n; ++i) {
			const geos::geom::Polygon* polygon = dynamic_cast<const geos::geom::Polygon*>(polygons->getGeometryN(i));
			getGeometryVertices(polygon, pnt, vertices, indices);
		}
	}
}

void SimpleLineSetter::getVertices(const SPFeatureI& feature, std::vector<qreal>& vertices, std::vector<int>& indices)const
{
    const UPGeometry& geometry = feature->geometry();
	Coordinate pnt = _layerManager->rootLayer()->viewEnvelope().center();
	if (feature->geometryType() == itPOLYGON) {
		getVerticesPolygon(feature, pnt, vertices, indices);
		return;
	}
    int n = (int)geometry->getNumGeometries();
    for(int  geom = 0; geom < n; ++geom ){
        const geos::geom::Geometry *subgeom = geometry->getGeometryN(geom);
        if (!subgeom)
            continue;
		getGeometryVertices(subgeom, pnt, vertices, indices);
     }
}

void SimpleLineSetter::getColors(const VisualAttribute &attr, const QVariant &value, const QColor &defaultColor, int start, std::vector<qreal>& colors) const
{
	QColor clr = attr.value2color(value);
	for (int j = start; j < colors.size(); j += 3) {
		if (value.isValid() && value.toInt() != iUNDEF) {
			if (defaultColor.isValid()) {
				colors[j] = defaultColor.redF();
				colors[j + 1] = defaultColor.greenF();
				colors[j + 2] = defaultColor.blueF();
			}
			else {
				colors[j] = clr.redF();
				colors[j + 1] = clr.greenF();
				colors[j + 2] = clr.blueF();
			}
		}
	}
}

