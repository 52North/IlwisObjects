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

LineLayerModel::LineLayerModel(LayerManager * manager, QStandardItem * parent, const QString & name, const QString & desc, const IOOptions& options) : VectorLayerModel(manager, parent, name, desc, options)
{
	_featureLayer = static_cast<FeatureLayerModel*>(parent);
	if (options["setter"].toString() == "")
		_linesetter.reset(new SimpleLineSetter(manager));
	if (_linesetter) {
		_linesetter->sourceCsySystem(_featureLayer->coverage()->coordinateSystem());
	}
	isSupportLayer(options.contains("support"));
	_isValid = _featureLayer->coverage().as<FeatureCoverage>()->featureCount(isSupportLayer() ? itPOLYGON :  itLINE) > 0;
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
	Vertices vertices;
	Colors colors; 
	VertexIndices indices;
    QColor clr = attr->value2color(value);
    if (clr.alphaF() == 1) {
		auto *geom = feature->geometry().get();

     	_linesetter->getVertices(geom, vertices, indices);
		colors.resize(vertices.size());
		for (int i = 0; i < vertices.size(); ++i) {
			colors[i].resize(vertices[i].size());
		}
		int start = 0; // std::max((int)0, (int)(colors.size() - 3));
		_linesetter->getColors(*attr, value, uicontext()->defaultColor("coverageline"), start, colors);
		for (int i = 0; i < indices.size(); ++i) {
			currentBuffer = _buffer.addObject(currentBuffer, vertices[i], indices[i], colors[i], itLINE, feature->featureid());
		}
    }
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
	if (type == "lines")
		return _buffer.bufferCount();
	return 0;
}

void LineLayerModel::fillAttributes()
{
	if (_featureLayer->coverage().isValid()) {
		_visualAttributes = QList<VisualAttribute *>();
		_visualAttributes.push_back(new LayerAttributeModel(this, _featureLayer->coverage(), DataDefinition()));

		// set default attribute
		if (_visualAttributes.size() > 0) {
			if (_visualAttributes.size() == 1)
				activeAttributeName(_visualAttributes.front()->attributename());
			else
				activeAttributeName(_visualAttributes[1]->attributename());
		}
        for (int layerIndex = 0; layerIndex < rowCount(); ++layerIndex) {
            LayerModel *lyr = static_cast<LayerModel *>(child(layerIndex));
            lyr->as<LayerModel>()->fillAttributes();
        }
	}
}

void LineLayerModel::vproperty(const QString &key, const QVariant &value) {
	VectorLayerModel::vproperty(key, value);

	if (key == "fixedlinecolor") {
		fixedColor(value.value<QColor>());
	}

	if (key == "linewidth") {
		linewidth(value.toDouble());
	}
}

QVariant LineLayerModel::vproperty(const QString &key) const {
	QVariant v = VectorLayerModel::vproperty(key);
	if (!v.isValid()) {
		if (key == "fixedlinecolor") {
			v.setValue(fixedColor());
		}
		else if (key == "linewidth") {
			v = _linewidth;
		}
	}
	return v;
}

void Ilwis::Ui::LineLayerModel::fixedColor(const QColor & clr)
{
	_fixedColor = clr;
	add2ChangedProperties("material");
}

QColor Ilwis::Ui::LineLayerModel::fixedColor() const
{
	return _fixedColor;
}

double Ilwis::Ui::LineLayerModel::linewidth() const
{
	return _linewidth;
}

void Ilwis::Ui::LineLayerModel::linewidth(double lw)
{
	if ( lw >= 0)
		_linewidth = lw;
}


LayerModel * Ilwis::Ui::LineLayerModel::create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options)
{
	return new LineLayerModel(manager, parentLayer, name, desc, options);
}


