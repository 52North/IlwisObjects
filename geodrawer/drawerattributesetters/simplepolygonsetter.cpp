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

#include <QApplication>
#include <QVector3D>
#include <QtGui/QOpenGLFunctions>
#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "geometryhelper.h"
#include "attributemodel.h"
#include "coveragedisplay/visualattribute.h"
#include "coveragedisplay/drawerattributesetterfactory.h"
#include "coveragedisplay/coveragelayermodel.h"
#include "coveragedisplay/layermanager.h"
#include "../featurelayermodel.h"
#include "../tesselation/ilwistesselator.h"
#include "simplepolygonsetter.h"


using namespace Ilwis;
using namespace Ui;

SimplePolygonSetter::SimplePolygonSetter(const LayerManager *manager) : BaseSpatialAttributeSetter(manager)
{

}

void SimplePolygonSetter::getVertices(const geos::geom::Geometry *geometry, Vertices& vertices, VertexIndices& indices) const
{
    IlwisTesselator tesselator;
    int n = (int)geometry->getNumGeometries();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    for(int  geom = 0; geom < n; ++geom ){
        const geos::geom::Geometry *subgeom = geometry->getGeometryN(geom);
        if (!subgeom)
            continue;
       tesselator.tesselate(subgeom, vertices, indices);
    }
    QApplication::restoreOverrideCursor();
}

void SimplePolygonSetter::getColors(const VisualAttribute &attr, const QVariant &value, const QColor &defaultColor, int start, Colors& colors) const
{
	QColor clr = attr.value2color(value);
	for (int i = 0; i < colors.size(); ++i) {
		for (int j = 0; j < colors.size(); j += 3) {
			if (value.isValid() && value.toInt() != iUNDEF) {
				colors[i][j] = clr.redF();
				colors[i][j + 1] = clr.greenF();
				colors[i][j + 2] = clr.blueF();
			}
			else {
				colors[i][j] = defaultColor.redF();
				colors[i][j + 1] = defaultColor.greenF();
				colors[i][j + 2] = defaultColor.blueF();
			}
		}
	}
}
