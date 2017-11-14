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
#include "drawerattributesetters\simplelinesetter.h"
#include "uicontextmodel.h"
#include "featurelayermodel.h"
#include "linelayermodel.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_LAYERMODEL("linelayer", LineLayerModel)

LineLayerModel::LineLayerModel()
{
}

LineLayerModel::LineLayerModel(LayerManager * manager, QObject * parent, const QString & name, const QString & desc, const IOOptions& options) : VectorLayerModel(manager, parent, name, desc, options)
{
	_featureLayer = static_cast<FeatureLayerModel*>(parent);
	if (options["setter"].toString() == "")
		_linesetter.reset(new SimpleLineSetter(manager));
	if (_linesetter) {
		_linesetter->sourceCsySystem(_featureLayer->coverage()->coordinateSystem());
	}
	_isValid = _featureLayer->coverage().as<FeatureCoverage>()->featureCount(options.contains("boundaries") ? itPOLYGON :  itLINE) > 0;
	_icon = "vector_line.png";
	_isDrawable = true;
	_layerType = itLINELAYER;

}

bool LineLayerModel::prepare(int prepType) {
	if (_featureLayer)
		return _featureLayer->prepare(prepType);
	return false;
}

QString LineLayerModel::value2string(const QVariant & value, const QString & attrName) const
{
	if (_featureLayer)
		return _featureLayer->value2string(value, attrName);

	return sUNDEF;
}

void LineLayerModel::addFeature(const SPFeatureI & feature, VisualAttribute *attr, const QVariant& value, int & currentBuffer)
{
	std::vector<qreal> vertices, colors;
	std::vector<int> indices;
	_linesetter->getVertices(feature, vertices, indices);
	colors.resize(vertices.size());
	int start = std::max((int)0, (int)(colors.size() - 3));
	_linesetter->getColors(*attr, value, uicontext()->defaultColor("coverageline"), start, colors);
	currentBuffer = _buffer.addObject(currentBuffer, vertices, indices, colors, itLINE, feature->featureid());
}

QVector<qreal> LineLayerModel::vertices(qint32 bufferIndex, const QString& ) const {
	if (bufferIndex <_buffer.bufferCount())
		return _buffer.buffer(bufferIndex)->_vertices;
	return QVector < qreal >();
}

QVector<int> LineLayerModel::indices(qint32 bufferIndex, const QString& ) const {
	if (bufferIndex < _buffer.bufferCount())
		return _buffer.buffer(bufferIndex)->_indices;
	return QVector<int>();
}

QVector<qreal> LineLayerModel::colors(qint32 bufferIndex, const QString& ) const {
	if (bufferIndex < _buffer.bufferCount())
		return _buffer.buffer(bufferIndex)->_colors;
	return QVector < qreal >();
}

int LineLayerModel::numberOfBuffers(const QString& type) const
{
	if (type == "linecoverage")
		return _buffer.bufferCount();
	return 0;
}


LayerModel * Ilwis::Ui::LineLayerModel::create(LayerManager *manager, LayerModel *layer, const QString &name, const QString &desc, const IOOptions& options)
{
	return new LineLayerModel(manager, layer, name, desc, options);
}


