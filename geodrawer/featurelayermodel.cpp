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

FeatureLayerModel::FeatureLayerModel(LayerManager *manager, QStandardItem *parent, const QString &name, const QString &desc, const IOOptions& options) :CoverageLayerModel( manager, parent,name, desc, options)
{
	_isDrawable = false;
	_isValid = true;
	_icon = "vector.png";
	_layerType = itFEATURELAYER;
    _hasFixedStructure = true;

	fillData();
}

void FeatureLayerModel::coverage(const ICoverage &cov)
{
    CoverageLayerModel::coverage(cov);
    _features = CoverageLayerModel::coverage().as<FeatureCoverage>();
	_icon = ResourceModel::iconPath(_features->ilwisType());
	if ( _features->featureCount(itPOINT) > 0)
		layerManager()->create(this, "pointlayer", layerManager(), "points of " + cov->name(), cov->description());
	if (_features->featureCount(itLINE) > 0)
		layerManager()->create(this, "linelayer", layerManager(), "lines of " + cov->name(), cov->description());
	if (_features->featureCount(itPOLYGON) > 0) {
        layerManager()->create(this, "arealayer", layerManager(), "areas of " + cov->name(), cov->description());
        layerManager()->create(this, "linelayer", layerManager(), "boundaries of " + cov->name(), cov->description(), { "support", true });

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
    for (int layerIndex = 0; layerIndex < rowCount(); ++layerIndex) {
        VectorLayerModel *vlayer = static_cast<VectorLayerModel *>(child(layerIndex));
		if (vlayer->layerType() == itPOINTLAYER)
			layers[itPOINT] = vlayer;
		if (vlayer->layerType() == itLINELAYER)
			layers[itLINE] = vlayer;
		if (vlayer->layerType() == itPOLYGONLAYER) {
			layers[itPOLYGON] = vlayer;
		}
	}

	VisualAttribute *attr = visualAttribute(activeAttributeName());

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
        for (auto layer : layers) {
            if (layer) {
                layer->resetBuffer(); // constructing a new vertices/indices so the internal buffers must be emptied
            }
        }
	
		PointLayerModel *points = 0;

        std::vector<quint64> ids;
		for (const SPFeatureI& feature : _features) {
			QVariant value = feature(columnIndex);
			IlwisTypes geomtype = feature->geometryType();
			if (layers[geomtype]) {
				auto *vlayer = layers[geomtype];
				vlayer->addFeature(feature, attr, value, currentBuffer[geomtype]);
                ids.push_back(feature->featureid());
				if (geomtype == itPOLYGON) {
					vlayer = layers[itLINE];
					vlayer->addFeature(feature, attr, value, currentBuffer[itLINE]);
				}
				if (points == 0 && vlayer->layerType() == itPOINTLAYER)
					points = static_cast<PointLayerModel *>(vlayer);
			}
		}
        for (auto layer : layers) {
            if (layer) {
                layer->finish(ids);
            }
        }

		_prepared |= (LayerModel::ptGEOMETRY | LayerModel::ptRENDER);

	} if (hasType(prepType, LayerModel::ptRENDER)) {
		if (!attr->isAbstractAttribute()) {
			int columnIndex = _features->attributeTable()->columnIndex(attr->attributename());
			for (const SPFeatureI& feature : _features) {
				QVariant value = feature(columnIndex);
				if (value.isValid()) {
					IlwisTypes geomtype = feature->geometryType();
					if (layers[geomtype]) {
						auto *vlayer = layers[geomtype];
						vlayer->setActiveFeatureColors(feature, attr, value);
					}
				}
			}
		}
	}
	return true;
}

LayerModel *FeatureLayerModel::create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options)
{
    return new FeatureLayerModel(manager, parentLayer, name, desc, options);
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
				activeAttributeName(_visualAttributes.front()->attributename());
			else
				activeAttributeName(_visualAttributes[1]->attributename());
		}
        for (int layerIndex = 0; layerIndex < rowCount(); ++layerIndex) {
            LayerModel *lyr = static_cast<LayerModel *>(child(layerIndex));
            lyr->fillAttributes();
        }
    }
}

int FeatureLayerModel::numberOfBuffers(const QString&) const {
    return 0;
}

QVariantList FeatureLayerModel::linkProperties() const
{
    QVariantList result = CoverageLayerModel::linkProperties();
    QVariantMap mp;
    mp["name"] = "feature";
    mp["modelid"] = modelId();
    result.push_back(mp);

    mp["name"] = "record";
    mp["modelid"] = modelId();
    mp["method"] = "selectionbyraw";
    result.push_back(mp);


    return result;
}

void FeatureLayerModel::linkAcceptMessage(const QVariantMap& parameters) {
    quint32 rec = parameters["identity"].toUInt();
    SPFeatureI feature = _features->feature(rec);
    if (feature) {
        for (int layerIndex = 0; layerIndex < rowCount(); ++layerIndex) {
            VectorLayerModel *vlayer = static_cast<VectorLayerModel *>(child(layerIndex));
            vlayer->addSelection(feature->featureid(), true);
        }
    }
}

void FeatureLayerModel::linkMessage(const QVariantMap& parms) {
    QVariantMap result;
    if (parms.contains("featureid")) {
        quint64 fid = parms["featureid"].toULongLong();
        for (const auto& feature : _features) {
            if (feature->featureid() == fid) {
                result["identity"] = fid;
                break;
            }
        }
        if (result.contains("identity")) {
            result["ilwisobjectid"] = _features->id();
            result["index"] = _features->index(_features->id());
            result["ilwisobjecttype"] = _features->ilwisType();
            emit linkSendMessage(result);
        }
    }
  
}

bool FeatureLayerModel::renderReady() {
    bool renderReady = true;
    for (int layerIndex = 0; layerIndex < rowCount(); ++layerIndex) {
        LayerModel *lyrchild = static_cast<LayerModel *>(child(layerIndex));
        renderReady &= lyrchild->renderReady();
    }
    return renderReady;
}

void FeatureLayerModel::renderReady(bool yesno) {
    for (int layerIndex = 0; layerIndex < rowCount(); ++layerIndex) {
        LayerModel *lyrchild = static_cast<LayerModel *>(child(layerIndex));
        lyrchild->renderReady(yesno);
    }
}

bool FeatureLayerModel::canUse(quint64 id)  {
	if (CoverageLayerModel::canUse(id))
		return true;
	return false;
}