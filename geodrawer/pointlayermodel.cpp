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
#include "drawerattributesetters\simplepointsetter.h"
#include "uicontextmodel.h"
#include "featurelayermodel.h"
#include "pointlayerModel.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_LAYERMODEL("pointlayer", PointLayerModel)

PointLayerModel::PointLayerModel()
{
}

PointLayerModel::PointLayerModel(LayerManager * manager, QStandardItem * parent, const QString & name, const QString & desc, const IOOptions& options) : VectorLayerModel(manager, parent, name, desc, options)
{
	_featureLayer = static_cast<FeatureLayerModel*>(parent);
	if (options["setter"].toString() == "")
		_pointsetter.reset(new SimplePointSetter(manager));
	if (_pointsetter) {
		_pointsetter->sourceCsySystem(_featureLayer->coverage()->coordinateSystem());
	}
	_isValid = _featureLayer->coverage().as<FeatureCoverage>()->featureCount(itPOINT) > 0;
	_icon = "vector_point.png";
	_isDrawable = true;
	_layerType = itPOINTLAYER;
}

QString PointLayerModel::value2string(const QVariant & value, const QString & attrName) const
{
	if (_featureLayer)
		return _featureLayer->value2string(value, attrName);

	return sUNDEF;
}

bool PointLayerModel::prepare(int prepType) {
	if (_featureLayer)
		return _featureLayer->prepare(prepType);
	return false;
}

void PointLayerModel::addFeature(const SPFeatureI & feature, VisualAttribute *attr, const QVariant& value, int & currentBuffer)
{
	std::vector<int> indices;
    QColor clr = attr->value2color(value);
    if (clr.alphaF() == 1) {
        _pointsetter->getVertices(feature->geometry().get(), _pointVertices, indices);
        _pointColors.resize(_pointVertices.size());
        int start = std::max((int)0, (int)(_pointColors.size() - 3));
        _pointsetter->getColors(*attr, value, uicontext()->defaultColor("coveragepoint"), start, _pointColors);
        //_buffer.addPoint(currentBuffer,_pointColors,  feature->featureid())
        //currentBuffer = _buffer.addObject(currentBuffer, _pointVertices, indices, _pointColors, itPOINT, feature->featureid());
    }
}

void PointLayerModel::setColors(int start, VisualAttribute *attr, const QVariant& value) {
	_pointsetter->getColors(*attr, value, uicontext()->defaultColor("coveragepoint"), start, _pointColors);
}

void Ilwis::Ui::PointLayerModel::finish(const std::vector<quint64>&ids)
{
	_buffer.addPoints(_pointVertices, _pointColors, ids);
	_pointColors = _pointVertices = std::vector<qreal>();
}

int PointLayerModel::numberOfBuffers(const QString& type) const
{
	if ( type == "points")
		return 1;
	return 0;
}

QVector<qreal> PointLayerModel::vertices(qint32 bufferIndex, const QString& ) const {
	if (bufferIndex <_buffer.bufferCount())
		return _buffer.buffer(0)->_vertices;
	return QVector < qreal >();
}

QVector<int> PointLayerModel::indices(qint32 bufferIndex, const QString& ) const {
	if (bufferIndex < _buffer.bufferCount())
		return _buffer.buffer(0)->_indices;
	return QVector<int>();
}

QVector<qreal> PointLayerModel::colors(qint32 bufferIndex, const QString& ) const {
	if (bufferIndex < _buffer.bufferCount())
		return _buffer.buffer(0)->_colors;
	return QVector < qreal >();
}

LayerModel * Ilwis::Ui::PointLayerModel::create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options)
{
	return new PointLayerModel(manager,parentLayer,name,desc, options);
}


