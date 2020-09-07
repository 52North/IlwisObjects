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
#include "drawerattributesetters\simplepolygonsetter.h"
#include "featurelayermodel.h"
#include "vectorlayermodel.h"

using namespace Ilwis;
using namespace Ui;


VectorLayerModel::VectorLayerModel(QObject *parent)
{
}


VectorLayerModel::VectorLayerModel(LayerManager * manager, QStandardItem * parent, const QString & name, const QString & desc, const IOOptions& options) : LayerModel(manager, parent, name, desc, options) {
	_isVector = true;
    _hasFixedStructure = true;
	add2ChangedProperties("layer");
}

QVariant VectorLayerModel::vproperty(const QString &attrName) const
{
	if (attrName == "id") {
		if (_featureLayer) {
			return _featureLayer->coverage()->id();
		}
	}
	return LayerModel::vproperty(attrName);
}

void VectorLayerModel::vproperty(const QString &attrName, const QVariant &value)
{
	LayerModel::vproperty(attrName, value);
}

void VectorLayerModel::fillAttributes()
{
	if (_featureLayer->_features.isValid()) {
		_visualAttributes = QList<VisualAttribute *>();
		_visualAttributes.push_back(new LayerAttributeModel(this, _featureLayer->_features, DataDefinition()));

		const FeatureAttributeDefinition& attributes = _featureLayer->_features->attributeDefinitions();
		for (int i = 0; i < attributes.columnCount(); ++i) {
			ColumnDefinition coldef = attributes.columndefinition(i);
			IlwisTypes valueType = coldef.datadef().domain()->valueType();
			if (hasType(valueType, itNUMBER | itDOMAINITEM | itSTRING | itCOLOR)) {
				_visualAttributes.push_back(new VisualAttribute(this, coldef.datadef(), coldef.name(), coldef.datadef().representation()));
				auto editors = uicontext()->propertyEditors(_visualAttributes.back(), _featureLayer->_features, coldef.datadef());
				for (auto editor : editors)
					_visualAttributes.back()->addVisualPropertyEditor(editor);
			}
		}
		// set default attribute
		if (_visualAttributes.size() > 0) {
			if (_visualAttributes.size() == 1)
				activeAttributeName(_visualAttributes.front()->attributename());
			else
				activeAttributeName(_visualAttributes[1]->attributename());
		}
	}
}

void VectorLayerModel::activeAttributeName(const QString& pName)
{
	try {
		if (pName == _activeAttribute)
			return;
		if (visualAttribute(pName) != 0)
			_activeAttribute = pName;
		emit activeAttributeChanged();
	}
	catch (const ErrorObject&) {

	}
}

void VectorLayerModel::addFeature(const SPFeatureI & feature, VisualAttribute * attr, const QVariant & value, int & currentBuffer)
{
}

int VectorLayerModel::numberOfBuffers(const QString &) const
{
	return 0;
}

bool Ilwis::Ui::VectorLayerModel::isCoverageBased() const
{
    return true;
}

void Ilwis::Ui::VectorLayerModel::resetBuffer()
{
    _buffer.clear();
}

ICoverage VectorLayerModel::coverage() const
{
    return _featureLayer->coverage();
}

void Ilwis::Ui::VectorLayerModel::finish(const std::vector<quint64>& ids)
{
}

void VectorLayerModel::addSelection(quint64 featureid, bool single)
{
    if (single) {
        _selectedFeatures = std::vector<quint64>();
        _selectedFeatures.push_back(featureid);
    }
    else {
        auto iter = std::find(_selectedFeatures.begin(), _selectedFeatures.end(), featureid);
        if (iter == _selectedFeatures.end()) {
            _selectedFeatures.push_back(featureid);
        }
    }
    add2ChangedProperties("buffers", true);
}

void VectorLayerModel::setActiveAttribute(int idx)
{
    if (idx < _visualAttributes.size()) {
        _featureLayer->setActiveAttribute(idx);
    }
}

void VectorLayerModel::setActiveFeatureColors(const SPFeatureI & feature, VisualAttribute *attr, const QVariant & value)
{
    QColor clr;
    if (_selectedFeatures.size() > 0) {
        auto iter = std::find(_selectedFeatures.begin(), _selectedFeatures.end(), feature->featureid());
        if (iter != _selectedFeatures.end()) {
            clr = QColor("red");
        }else
            clr = attr->value2color(value);
    }else
	    clr = attr->value2color(value);
	_buffer.changeColor(feature->featureid(), clr);
}

bool VectorLayerModel::renderReady()
{
    return _renderReady;
}

void VectorLayerModel::renderReady(bool yesno)
{
    _renderReady = yesno;
}

QColor VectorLayerModel::color(const IRepresentation &rpr, const QString& propName, double value, LayerModel::ColorValueMeaning cvm) {
	if (_activeAttribute != sUNDEF) {
		VisualAttribute* attr = visualAttribute(_activeAttribute);
		if (!attr)
			return QColor();

		if (cvm == LayerModel::cvmFRACTION) {
			NumericRange numrange = attr->actualRange();
			value = numrange.min() + numrange.distance() * value;

		}
		QColor clr = attr->value2color(value);
		return clr;
	}
	return QColor();
}