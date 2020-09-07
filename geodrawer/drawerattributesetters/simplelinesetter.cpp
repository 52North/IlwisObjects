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

void SimpleLineSetter::getGeometryVertices(const geos::geom::Geometry* geom, Vertices& vertices, VertexIndices& indices) const {

	if (!geom)
		return;

	const std::vector<geos::geom::Coordinate>* coords = geom->getCoordinates()->toVector();

	Coordinate crd;
	quint64 verticesIndex = indices.size() > 0 ? indices.back().back() + 1 : 0;
	int mm = MAX_INDICES_BUFFER;
    int count = 0;
	for (int i = 0; i < coords->size(); ++i) {
		if (verticesIndex > mm || indices.size() == 0) {
			indices.resize(indices.size() + 1);
			vertices.resize(vertices.size() + 1);
			verticesIndex = 0;
			if (i != 0) { // we are breaking the somewhere in the line; need to copy the last coordinate to the new vertices plus new index
				auto& prevVertices = vertices[vertices.size() - 2];
				int last = prevVertices.size() - 1;
				vertices.back().push_back(prevVertices[last - 2]);
				vertices.back().push_back(prevVertices[last - 1]);
				vertices.back().push_back(prevVertices[last]);
				indices.back().push_back(verticesIndex++);
				count = 1;
			}

		}
		if (count > 1)
			indices.back().push_back(indices.back().back());

		crd = coords->at(i);
		if (coordinateConversionNeeded()) {
			crd = _targetSystem->coord2coord(_sourceSystem, crd);
		}
		vertices.back().push_back(crd.x);
		vertices.back().push_back(crd.y);
		vertices.back().push_back((std::isnan(crd.z) || crd.z == rUNDEF) ? 0 : crd.z);
		indices.back().push_back(verticesIndex);
		++verticesIndex;
		++count;
	}
}

void SimpleLineSetter::getVerticesPolygon(const geos::geom::Geometry *polygons, Vertices& vertices, VertexIndices& indices) const {
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

void SimpleLineSetter::getVertices(const geos::geom::Geometry *geometry, Vertices& vertices, VertexIndices& indices) const
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

void SimpleLineSetter::getColors(const VisualAttribute &attr, const QVariant &value, const QColor &defaultColor, int start, Colors& colors) const
{

	QColor clr = attr.value2color(value);
	for (int i = 0; i < colors.size(); ++i) {
		for (int j = start; j < colors[i].size(); j += 3) {
			if (value.isValid() && value.toInt() != iUNDEF) {
				colors[i][j] = clr.redF();
				colors[i][j + 1] = clr.greenF();
				colors[i][j + 2] = clr.blueF();
			}
		}
	}
}

