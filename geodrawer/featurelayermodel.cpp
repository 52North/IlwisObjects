#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coverage.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "uicontextmodel.h"
#include "coveragedisplay\layermanager.h"
#include "representation.h"
#include "coveragedisplay/coveragelayermodel.h"
#include "feature.h"
#include "featurecoverage.h"
#include "featureiterator.h"
#include "vertexiterator.h"
#include "coveragedisplay\visualattribute.h"
#include "coveragedisplay/oglbuffer.h"
#include "drawerattributesetters\basespatialattributesetter.h"
#include "drawerattributesetters\simplelinesetter.h"
#include "drawerattributesetters\simplepolygonsetter.h"
#include "drawerattributesetters\simplepointsetter.h"
#include "uicontextmodel.h"
#include "pointlayermodel.h"
#include "linelayermodel.h"
#include "polygonlayermodel.h"
#include "featurelayermodel.h"

using namespace Ilwis;
using namespace Ui; 

REGISTER_LAYERMODEL("featurecoverage",FeatureLayerModel)

Ilwis::Ui::FeatureLayerModel::FeatureLayerModel() : CoverageLayerModel()
{
}

FeatureLayerModel::FeatureLayerModel(LayerManager *manager, QObject *parent, const QString &name, const QString &desc, const IOOptions& options) :CoverageLayerModel( manager, parent,name, desc, options)
{
	_isDrawable = false;
	_isValid = true;
	_icon = "vector.png";
	_layerType = itFEATURELAYER;

	fillData();
}

void FeatureLayerModel::coverage(const ICoverage &cov)
{
    CoverageLayerModel::coverage(cov);
    _features = CoverageLayerModel::coverage().as<FeatureCoverage>();
	_icon = ResourceModel::iconPath(_features->ilwisType());
	if ( _features->featureCount(itPOINT) > 0)
		layersManager()->create(this, "pointlayer", layersManager(), "points of " + cov->name(), cov->description());
	if (_features->featureCount(itLINE) > 0)
		layersManager()->create(this, "linelayer", layersManager(), "lines of " + cov->name(), cov->description());
	if (_features->featureCount(itPOLYGON) > 0) {
		layersManager()->create(this, "arealayer", layersManager(), "areas of " + cov->name(), cov->description());
		layersManager()->create(this, "linelayer", layersManager(), "boundaries of " + cov->name(), cov->description(), { "boundaries", true });
	}
		
    fillAttributes();

}

ICoverage Ilwis::Ui::FeatureLayerModel::coverage() const
{
	return CoverageLayerModel::coverage();
}

QVariant FeatureLayerModel::vproperty(const QString &attrName) const
{
	return CoverageLayerModel::vproperty(attrName);

}

void FeatureLayerModel::vproperty(const QString &attrName, const QVariant &value)
{
	CoverageLayerModel::vproperty(attrName, value);
}

QString FeatureLayerModel::value2string(const QVariant &value, const QString &attrName) const
{
    if ( attrName != "") {
            IFeatureCoverage features = CoverageLayerModel::coverage().as<FeatureCoverage>();
            ColumnDefinition coldef = features->attributeDefinitions().columndefinition(attrName);
            return v2s(coldef, value);

    }
    if ( value.toDouble() == rUNDEF)
        return sUNDEF;

    return QString::number(value.toDouble(), 'f', 3);
}


bool FeatureLayerModel::prepare(int prepType)
{
	std::vector<VectorLayerModel *> layers(5, 0);
	auto layersList = children();
	for (auto *layer : layersList) {
		auto *vlayer = static_cast<VectorLayerModel *>(layer);
		if (vlayer->layerType() == itPOINTLAYER)
			layers[itPOINT] = vlayer;
		if (vlayer->layerType() == itLINELAYER)
			layers[itLINE] = vlayer;
		if (vlayer->layerType() == itPOLYGONLAYER) {
			layers[itPOLYGON] = vlayer;
		}
	}

	int featureIndex = 0;
	VisualAttribute *attr = visualAttribute(activeAttribute());

	if (hasType(prepType, LayerModel::ptGEOMETRY) && !isPrepared(LayerModel::ptGEOMETRY)) {
		if (!_features.isValid()) {
			return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "FeatureCoverage", TR("Visualization"));
		}
		// set all to 0
		// get a description of how to render
		
		int columnIndex = _features->attributeTable()->columnIndex(attr->attributename());

		_features->loadData();
		_featureDrawings.resize(_features->featureCount());
		int currentBuffer[5] = { 0,0,0,0,0 };
	
		PointLayerModel *points = 0;
		for (const SPFeatureI& feature : _features) {
			QVariant value = feature(columnIndex);
			IlwisTypes geomtype = feature->geometryType();
			if (layers[geomtype]) {
				auto *vlayer = layers[geomtype];
				vlayer->addFeature(feature, attr, value, currentBuffer[geomtype]);
				if (points == 0 && vlayer->layerType() == itPOINTLAYER)
					points = static_cast<PointLayerModel *>(vlayer);
			}
			++featureIndex;
		}
		if (points) {
			points->finishAddingPoints();
		}
		_prepared |= (LayerModel::ptGEOMETRY | LayerModel::ptRENDER);

	} if (hasType(prepType, LayerModel::ptRENDER)) {
		int columnIndex = _features->attributeTable()->columnIndex(attr->attributename());
		for (const SPFeatureI& feature : _features) {
			QVariant value = feature(columnIndex);
			IlwisTypes geomtype = feature->geometryType();
			if (layers[geomtype]) {
				auto *vlayer = layers[geomtype];

				//vlayer->setColors(0, feature, attr, value);
			}
			++featureIndex;
		}
	}
	return true;
}

LayerModel *FeatureLayerModel::create(LayerManager *manager, LayerModel *layer, const QString &name, const QString &desc, const IOOptions& options)
{
    return new FeatureLayerModel(manager, layer, name, desc, options);
}

void FeatureLayerModel::fillAttributes()
{
    if ( _features.isValid()){
        _visualAttributes = QList<VisualAttribute *>();
        _visualAttributes.push_back(new LayerAttributeModel(this,_features,DataDefinition()));

        const FeatureAttributeDefinition& attributes = _features->attributeDefinitions();
        for(int i=0; i < attributes.columnCount(); ++i){
            ColumnDefinition coldef = attributes.columndefinition(i);
            IlwisTypes valueType = coldef.datadef().domain()->valueType();
            if ( hasType(valueType, itNUMBER|itDOMAINITEM|itSTRING|itCOLOR)){
                _visualAttributes.push_back(new VisualAttribute(this, coldef.datadef(),coldef.name()));
				auto editors =uicontext()->propertyEditors(_visualAttributes.back(), _features, coldef.datadef());
				for (auto editor : editors)
					_visualAttributes.back()->addVisualPropertyEditor(editor);
            }
        }
		// set default attribute
		if (_visualAttributes.size() > 0) {
			if (_visualAttributes.size() == 1)
				activeAttribute(_visualAttributes.front()->attributename());
			else
				activeAttribute(_visualAttributes[1]->attributename());
		}
		auto layers = children();
		for(auto *layer : layers)
			layer->as<LayerModel>()->fillAttributes();
    }
}






