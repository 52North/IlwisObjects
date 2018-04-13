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


