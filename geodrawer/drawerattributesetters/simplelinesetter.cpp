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

void SimpleLineSetter::getVertices(const SPFeatureI& feature, std::vector<qreal>& vertices, std::vector<int>& indices)const
{
    const UPGeometry& geometry = feature->geometry();
	Coordinate pnt = _layerManager->rootLayer()->viewEnvelope().center();
    int n = (int)geometry->getNumGeometries();
    for(int  geom = 0; geom < n; ++geom ){
        const geos::geom::Geometry *subgeom = geometry->getGeometryN(geom);
        if (!subgeom)
            continue;
        auto *coords = subgeom->getCoordinates();

        Coordinate crd;
		int extraIndices = (int)coords->size() * 2 - 2;
		int currentIndicesSize = (int)indices.size();
		int verticesIndex = (int)vertices.size() / 3;
		int indicesIndex = currentIndicesSize;
		indices.resize(currentIndicesSize + extraIndices);
        for(int i = 0; i < coords->size() ; ++i){
			if (i > 1)
				indices[indicesIndex++] = verticesIndex + i - 1;

            coords->getAt(i, crd);
            if ( coordinateConversionNeeded()){
                crd = _targetSystem->coord2coord(_sourceSystem, crd);
            }
			vertices.push_back(crd.x - pnt.x);
			vertices.push_back(crd.y - pnt.y);
			vertices.push_back((std::isnan(crd.z) || crd.z == rUNDEF) ? 0 : crd.z);
			indices[indicesIndex++] = verticesIndex + i;
        }
        delete coords;
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

