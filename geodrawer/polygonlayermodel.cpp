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

#include "kernel.h"
#include <QFileInfo>
#include <QFile>
#include "ilwisdata.h"
#include "geometries.h"
#include "coverage.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "uicontextmodel.h"
#include "coveragedisplay\layermanager.h"
#include "coveragedisplay/coveragelayermodel.h"
#include "feature.h"
#include "featurecoverage.h"
#include "coveragedisplay\visualattribute.h"
#include "drawerattributesetters\basespatialattributesetter.h"
#include "drawerattributesetters\simplepolygonsetter.h"
#include "uicontextmodel.h"
#include "featurelayermodel.h"
#include "polygonlayermodel.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_LAYERMODEL("linelayer", PolygonLayerModel)
REGISTER_LAYERMODEL2("arealayer", PolygonLayerModel)

PolygonLayerModel::PolygonLayerModel()
{
}

PolygonLayerModel::PolygonLayerModel(LayerManager * manager, QStandardItem * parent, const QString & name, const QString & desc, const IOOptions& options) : VectorLayerModel(manager, parent, name, desc, options)
{
	_featureLayer = static_cast<FeatureLayerModel*>(parent);
	if (options["setter"].toString() == "")
		_polygonsetter.reset(new SimplePolygonSetter(manager));
	if (_polygonsetter) {
		_polygonsetter->sourceCsySystem(_featureLayer->coverage()->coordinateSystem());
	}
	_isValid = _featureLayer->coverage().as<FeatureCoverage>()->featureCount(itPOLYGON) > 0;
	_icon = "vector_fill.png";
	_isDrawable = true;
	_isVector = true;
	_layerType = itPOLYGONLAYER;
}

bool PolygonLayerModel::prepare(int prepType) {
	if (_featureLayer)
		return _featureLayer->prepare(prepType);

	return false;
}

QString PolygonLayerModel::value2string(const QVariant & value, const QString & attrName) const
{
	if (_featureLayer)
		return _featureLayer->value2string(value, attrName);

	return sUNDEF;
}

void PolygonLayerModel::addFeature(const SPFeatureI & feature, VisualAttribute *attr, const QVariant& value, int & currentBuffer)
{

    QColor clr = attr->value2color(value);
    if (clr.alphaF() == 1) {

        if (!_buffer.loadTriangulation()) {
            const auto& geometry = feature->geometry();
            int n = (int)geometry->getNumGeometries();
            for (int geom = 0; geom < n; ++geom) {
                std::vector<qreal> vertices, colors;
                std::vector<int> indices;

                const geos::geom::Geometry *subgeom = geometry->getGeometryN(geom);
                if (!subgeom)
                    continue;
                _polygonsetter->getVertices(subgeom, vertices, indices);

                colors.resize(vertices.size());

                _polygonsetter->getColors(*attr, value, uicontext()->defaultColor("coveragearea"), 0, colors);
                currentBuffer = _buffer.addObject(currentBuffer, vertices, indices, colors, itPOLYGON, feature->featureid());
            }
        }
    }
}

VisualAttribute * Ilwis::Ui::PolygonLayerModel::activeAttribute()
{
    if (_featureLayer) {
        return _featureLayer->activeAttribute();
    }
    return 0;
}

void Ilwis::Ui::PolygonLayerModel::finish(const std::vector<quint64>& ids)
{
    if (!_buffer.loadTriangulation()) // if there wasn't a triangulation file
        _buffer.storeTriangulation(coverage()->resource().url());
    else
        _buffer.map(ids);
    const ICoordinateSystem & csyGeom = _featureLayer->coverage()->coordinateSystem();
    const ICoordinateSystem & csyRoot = layerManager()->rootLayer()->screenCsy();
    if ( csyGeom.isValid() && csyRoot.isValid() && csyGeom != csyRoot) {
        for (qint32 bufferIndex = 0; bufferIndex < _buffer.bufferCount(); ++bufferIndex) {
            QVector<qreal> & vertices = _buffer.vertices(bufferIndex);
            for (quint64 vi = 0; vi < vertices.size(); vi += 3) {
                Coordinate crd(vertices[vi], vertices[vi + 1], vertices[vi + 2]);
                Coordinate crdTransformed = csyRoot->coord2coord(csyGeom,crd);
                vertices[vi] = crdTransformed.x;
                vertices[vi + 1] = crdTransformed.y;
                vertices[vi + 2] = (std::isnan(crdTransformed.z) || crdTransformed.z == rUNDEF) ? 0 : crdTransformed.z;
            }
        }
    }
}

int PolygonLayerModel::numberOfBuffers(const QString& type) const
{
	if ( type == "polygons")
		return _buffer.bufferCount();
	return 0;
}

QVector<qreal> PolygonLayerModel::vertices(qint32 bufferIndex, const QString& ) const {
	if (bufferIndex <_buffer.bufferCount())
		return _buffer.buffer(bufferIndex)->_vertices;
	return QVector < qreal >();
}

QVector<int> PolygonLayerModel::indices(qint32 bufferIndex, const QString& ) const {
	if (bufferIndex < _buffer.bufferCount())
		return _buffer.buffer(bufferIndex)->_indices;
	return QVector<int>();
}

QVector<qreal> PolygonLayerModel::colors(qint32 bufferIndex, const QString& ) const {
	if (bufferIndex < _buffer.bufferCount())
		return _buffer.buffer(bufferIndex)->_colors;
	return QVector < qreal >();
}

LayerModel * PolygonLayerModel::create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options)
{
	return new PolygonLayerModel(manager, parentLayer, name, desc, options);
}

void PolygonLayerModel::resetBuffer()
{
    VectorLayerModel::resetBuffer();
    _buffer.loadTriangulation(_featureLayer->coverage()->resource().url());
}


