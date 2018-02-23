#include "kernel.h"
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

PolygonLayerModel::PolygonLayerModel(LayerManager * manager, QObject * parent, const QString & name, const QString & desc, const IOOptions& options) : VectorLayerModel(manager, parent, name, desc, options)
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
	std::vector<qreal> vertices, colors;
	std::vector<int> indices;
    QColor clr = attr->value2color(value);
    if (clr.alphaF() == 1) {
        _polygonsetter->getVertices(feature, vertices, indices);
        colors.resize(vertices.size());
        int start = std::max((int)0, (int)(colors.size() - 3));
        _polygonsetter->getColors(*attr, value, uicontext()->defaultColor("coveragearea"), start, colors);
        currentBuffer = _buffer.addObject(currentBuffer, vertices, indices, colors, itPOLYGON, feature->featureid());
    }
}

VisualAttribute * Ilwis::Ui::PolygonLayerModel::activeAttribute()
{
    if (_featureLayer) {
        return _featureLayer->activeAttribute();
    }
    return 0;
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

LayerModel * Ilwis::Ui::PolygonLayerModel::create(LayerManager *manager, LayerModel *layer, const QString &name, const QString &desc, const IOOptions& options)
{
	return new PolygonLayerModel(manager, layer, name, desc, options);
}


