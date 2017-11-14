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

void SimplePointSetter::getVertices(const SPFeatureI &feature, std::vector<qreal>& vertices, std::vector<int>& ) const
{
	Envelope env = _layerManager->rootLayer()->vproperty("coverageenvelope").value<Envelope>();
	Coordinate pnt = _layerManager->rootLayer()->viewEnvelope().center();
	const UPGeometry& geometry = feature->geometry();
	int n = (int)geometry->getNumGeometries();
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
		vertices.push_back(coord.x - pnt.x);
		vertices.push_back(coord.y - pnt.y);
		vertices.push_back(z);
	}
}

void SimplePointSetter::getColors(const VisualAttribute &attr, const QVariant &value, const QColor &defaultColor, int start, std::vector<qreal>& colors) const
{
	QColor clr = attr.value2color(value);
	int last = std::max((int)0,(int)(colors.size() - 3));
	for (int j = last; j < colors.size(); j+=3) {
		if (value.isValid() && value.toInt() != iUNDEF) {
			colors[j] = clr.redF();
			colors[j+1] =clr.greenF();
			colors[j+2] = clr.blueF();
		}
	}
}

