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
#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "geometries.h"
#include "geometryhelper.h"
#include "attributemodel.h"
#include "coveragedisplay/visualattribute.h"
#include "coveragedisplay/drawerattributesetterfactory.h"
#include "coveragedisplay/coveragelayermodel.h"
#include "../featurelayermodel.h"
#include "coveragedisplay/layermanager.h"
#include "simplepointsetter.h"

using namespace Ilwis;
using namespace Ui;

SimplePointSetter::SimplePointSetter(const LayerManager *manager) : BaseSpatialAttributeSetter(manager)
{

}

void SimplePointSetter::getVertices(const geos::geom::Geometry *geometry, Vertices& vertices, VertexIndices& ) const
{
	Envelope env = _layerManager->rootLayer()->vproperty("coverageenvelope").value<Envelope>();
	int n = (int)geometry->getNumGeometries();
	if (vertices.size() == 0)
		vertices.resize(1);
	for (int geom = 0; geom < n; ++geom) {
		const geos::geom::Geometry *subgeom = geometry->getGeometryN(geom);
		if (!subgeom)
			continue;
		const geos::geom::Coordinate *crd = subgeom->getCoordinate();
		Coordinate coord = *crd;
		if (coordinateConversionNeeded()) {
			coord = _targetSystem->coord2coord(_sourceSystem, *crd);
		}

		double z = coord.z == rUNDEF || std::isnan(coord.z) ? 0 : coord.z;
		vertices[0].push_back(coord.x);
		vertices[0].push_back(coord.y);
		vertices[0].push_back(z);
	}
}

void SimplePointSetter::getColors(const VisualAttribute &attr, const QVariant &value, const QColor &defaultColor, int start, Colors& colors) const
{
	QColor clr = attr.value2color(value);
	for (int i = 0; i < colors.size(); ++i) {
		int last = std::max((int)0, (int)(colors[i].size() - 3));
		for (int j = last; j < colors.size(); j += 3) {
			if (value.isValid() && value.toInt() != iUNDEF) {
				colors[i][j] = clr.redF();
				colors[i][j + 1] = clr.greenF();
				colors[i][j + 2] = clr.blueF();
			}
		}
	}
}

