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
				_visualAttributes.push_back(new VisualAttribute(this, coldef.datadef(), coldef.name()));
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

void VectorLayerModel::setActiveAttribute(int idx)
{
	if (idx < _visualAttributes.size()) {
		_featureLayer->setActiveAttribute(idx);
	}
}

void VectorLayerModel::setActiveFeatureColors(const SPFeatureI & feature, VisualAttribute *attr, const QVariant & value)
{
	QColor clr = attr->value2color(value);
	_buffer.changeColor(feature->featureid(), clr);
}

