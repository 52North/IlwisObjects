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

void SimplePolygonSetter::getVertices(const geos::geom::Geometry *geometry, std::vector<qreal>& vertices, std::vector<int>& indices) const
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

void SimplePolygonSetter::getColors(const VisualAttribute &attr, const QVariant &value, const QColor &defaultColor, int start, std::vector<qreal>& colors) const
{
	QColor clr = attr.value2color(value);
    for(int j =0; j < colors.size(); j+=3){
        if ( value.isValid() && value.toInt() != iUNDEF) {
			colors[j] =clr.redF();
			colors[j+1] =clr.greenF();
			colors[j+2] =clr.blueF();
		}
		else {
			colors[j] = defaultColor.redF();
			colors[j + 1] = defaultColor.greenF();
			colors[j + 2] = defaultColor.blueF();
		}
    }
}
