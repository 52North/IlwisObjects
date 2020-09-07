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

#include "layermodel.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "coverage.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "layermanager.h"
#include "representation.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "layermodel.h"
#include "modelregistry.h"
#include "tree.h"

using namespace Ilwis;
using namespace Ui;

//REGISTER_LAYERMODEL("layermodel",LayerModel)

Ilwis::Ui::LayerModel::LayerModel()
{
}

LayerModel::LayerModel(LayerManager *manager, QStandardItem *parent, const QString& name, const QString& desc, const IOOptions& opt) : QStandardItem(name), _layerManager(manager){
	if (parent) {
		if (parent->index().isValid()) { // the invisiblerootitem has by definition an invalid index, so we can not use it as parent, but this parent is only true for all layers directly under the root. The layermanager can then act as parent
			setParent(static_cast<LayerModel *>(parent));
		}
		else
			setParent(manager);
	}
    _modelId = modelregistry()->newModelId();
	if (opt.contains("virtual")) {
		_isVirtual = opt["virtual"].toBool();
	}
        
}

Ilwis::Ui::LayerModel::~LayerModel()
{
}

LayerModel *LayerModel::create(LayerManager *manager, LayerModel *layer, const QString &name, const QString &desc, const IOOptions& options)
{
    //return new LayerModel(manager, layer, name, desc, options);

    return 0;
}

bool LayerModel::isVirtual() const {
	return _isVirtual;
}

void LayerModel::redraw() const
{

}

QVariant LayerModel::vproperty(const QString &key) const
{
	QVariant var;
	QVariant parentValue;
	if ( parentLayer())
		parentValue = parentLayer()->vproperty(key);

	if (key == "opacity") {
		double pv = parentValue.isValid() ? parentValue.toDouble() : 1.0;
		var = _opacity * pv;
	} if (key == "active") {
		bool pv = parentValue.isValid() ? parentValue.toBool() : true;
		var = pv && _active;
    } if (key == "color") {
        return "0x000000"; // default color is black, other layers may overrule this
    }

    if ( key.indexOf("visualattribute") == 0){
        QStringList parts = key.split("|");
        if ( parts.size() >= 3){
            auto *attr = visualAttribute(parts[2]);
            if ( attr){
                if ( parts[1] == "representation")
                    var.setValue<IRepresentation>(attr->representation());
                else if ( parts[1] == "stretchrange"){
                    var.setValue<NumericRange>(attr->stretchRange());
                } if (parts[1] == "actualrange") {
					var.setValue<NumericRange>(attr->actualRange());
				} else if ( parts[1] == "domain"){
                    var.setValue<IDomain>(attr->attributeDomain());
                }
				else if (parts[1] == "color") {
					var.setValue<QColor>(attr->value2color(parts[3].toInt()));
				}
            }
        }
    }
    return var;
}

void LayerModel::vproperty(const QString &key, const QVariant &value)
{
    if ( key == "opacity")
       opacity(value.toFloat());
	if (key == "active") {
		active(value.toBool());
        for (int layerIndex = 0; layerIndex < rowCount(); ++layerIndex){
            LayerModel *lyr = static_cast<LayerModel *>(child(layerIndex));
			lyr->vproperty(key, value);
		}
	}


    if ( key.indexOf("visualattribute") == 0){
        QStringList parts = key.split("|");
		VisualAttribute * vattr = visualAttribute(parts[1]);
		if (!vattr)
			return;

        if ( parts.size() ==3){
            if ( parts[2] == "representation"){
				IRepresentation rpr(value.toString());
                if ( rpr.isValid()){
                    vattr->representation(rpr);
                }
            }else if( parts[2] == "domain"){
				IDomain dom(value.toString());
                if ( dom.isValid()){
                    vattr->domain(dom);
                }
			}
			else if (parts[2] == "stretchrange") {

				if (parts.size() == 3) {
					NumericRange rng(value.toString());
					vattr->stretchRange(rng);
				} 
			}
        }
    }
}

void LayerModel::addVisualAttribute(VisualAttribute *attr)
{
    if ( !visualAttribute(attr->attributename())){
        _visualAttributes.append(attr);
        emit visualAttributesChanged();
    }
}

QString LayerModel::layerData(const Coordinate &crdIn, const QString &attrName, QVariantList &items) const
{
    QString alltext;
    for (int layerIndex = 0; layerIndex < rowCount(); ++layerIndex) {
        LayerModel *lyr = static_cast<LayerModel *>(child(layerIndex));
        if (!lyr)
            continue;
        QString txt = lyr->layerData(crdIn, attrName, items);
        if ( alltext != "")
            alltext += ";";
        alltext += txt;

    }
    return alltext;
}

QString LayerModel::icon() const
{
    return _icon;
}

IlwisTypes LayerModel::layerType() const
{
	return _layerType;
}

QString Ilwis::Ui::LayerModel::drawType() const
{
	switch (layerType()) {
	case itPOINTLAYER:
		return "point";
	case itLINELAYER:
		return "line";
	case itPOLYGONLAYER:
		return "polygon";
	case itRASTERLAYER:
		return "raster";
    case itGRIDLAYER:
        return "line";
	}
	return sUNDEF;
}

int LayerModel::numberOfBuffers(const QString&) const
{
	return 0;
}

QVector<qreal> LayerModel::vertices(qint32 index, const QString& tp) const
{
	return QVector<qreal>();
}

Q_INVOKABLE QVector<int> LayerModel::indices(qint32 bufferIndex, const QString& tp) const
{
	return QVector<int>();
}


QVector<qreal> LayerModel::colors(qint32 index, const QString& tp) const
{
	return QVector<qreal>();
}

const LayerManager *LayerModel::layerManager() const
{
    return _layerManager;
}

LayerManager * Ilwis::Ui::LayerModel::layerManager()
{
	return _layerManager;
}


QQmlListProperty<VisualAttribute> LayerModel::vattributes()
{
    return QQmlListProperty<VisualAttribute>(this, _visualAttributes);
}

int LayerModel::activeVProperty() const
{
    return _activeVProperty;
}

void LayerModel::activeVProperty(int index)
{
  if ( index < _visualAttributes.size() && index >= 0){
      _activeVProperty = index;
  }
}

QString LayerModel::visualVPropertyByIndex(int index) const
{
    if ( index < _visualAttributes.size()){
       return _visualAttributes[index]->attributename();
    }
    return sUNDEF;
}

void LayerModel::addLayer(LayerModel *layer)
{
    appendRow(layer);
}

VisualAttribute *LayerModel::visualAttribute(const QString &attrName) const
{
    for(VisualAttribute *attr : _visualAttributes){
        if ( attr->attributename() == attrName)
            return attr;
    }
    return 0;
}

void Ilwis::Ui::LayerModel::order(qint32 n)
{
    _order = n;
}

qint32 Ilwis::Ui::LayerModel::order() const
{
    return _order;
}

bool Ilwis::Ui::LayerModel::isCoverageBased() const
{
    return false;
}

bool Ilwis::Ui::LayerModel::hasFixedStructure() const
{
    return _hasFixedStructure;
}

LayerModel * LayerModel::findLayer(int nodeid)
{
    for (int layerIndex = 0; layerIndex < rowCount(); ++layerIndex) {
        LayerModel *layer = static_cast<LayerModel *>(child(layerIndex));
        if (layer->nodeId() == nodeid)
            return layer;
        layer = layer->findLayer(nodeid);
        if (layer)
            return layer;
    }
    return 0;
}

qint32 LayerModel::nodeId() const
{
    return _nodeid;
}

void LayerModel::nodeId(qint32 id)
{
    _nodeid = id;
}

void LayerModel::addSelection(quint64 featureid, bool single)
{
    for (int layerIndex = 0; layerIndex < rowCount(); ++layerIndex) {
        LayerModel *vlayer = static_cast<LayerModel *>(child(layerIndex));
        vlayer->addSelection(featureid, true);
    }
}

void Ilwis::Ui::LayerModel::activeAttributeName(const QString & pName)
{
}

VisualAttribute * Ilwis::Ui::LayerModel::activeAttribute()
{
    return nullptr;
}

const VisualAttribute * Ilwis::Ui::LayerModel::activeAttribute() const
{
	return nullptr;
}

void Ilwis::Ui::LayerModel::isSupportLayer(bool yesno)
{
	_isSupportLayer = yesno;
}

double LayerModel::opacity() const
{
    return _opacity;
}

void LayerModel::opacity(double opacity)
{
	if (opacity >= 0 && opacity <= 1.0) {
		if (opacity != _opacity) {
			_opacity = opacity;
			add2ChangedProperties(isSupportLayer() ? "material" : "layer",rowCount() > 0);
			emit opacityChanged();
		}
	}
}

const LayerModel *LayerModel::parentLayer() const
{
    const QStandardItem *lyr = dynamic_cast<const QStandardItem *>(this);
    if (lyr->index().isValid()) {
        auto *p1 = lyr->parent();
        auto *p2 = _layerManager->layerTree()->invisibleRootItem();
        if ( p1 != p2) // the top of the hierarchy is not a layer but a plain qstandarditem. So we can not return it as a layer
            return static_cast<const LayerModel *>(p1);
    }
    return 0;
}

LayerModel *LayerModel::parentLayer() {
    QStandardItem *lyr = dynamic_cast<QStandardItem *>(this);
    if (lyr->index().isValid()) {
        auto *p1 = lyr->parent();
        auto *p2 = _layerManager->layerTree()->invisibleRootItem();
        if (p1 != p2)
            return static_cast<LayerModel *>(lyr->parent());
    }
    return 0;
}

void LayerModel::clearLayers()
{
    removeRows(0, rowCount());
    _visualAttributes = QList<VisualAttribute *>();
	_changedProperties = std::set<QString>();
	_geometryChanged = true;
	_meshes = std::vector<quint32>();

}

LayerModel * Ilwis::Ui::LayerModel::findLayerByName(const QString & name)
{
    for (int layerIndex = 0; layerIndex < rowCount(); ++layerIndex) {
        LayerModel *lyr = static_cast<LayerModel *>(child(layerIndex));
		if (lyr->text() == name) {
			return lyr;
		}
		else {
			LayerModel *child = lyr->findLayerByName(name);
			if (child)
				return child;
		}
	}
	return 0;
}

const LayerModel * Ilwis::Ui::LayerModel::findLayerByName(const QString & name) const
{
    for (int layerIndex = 0; layerIndex < rowCount(); ++layerIndex) {
        LayerModel *lyr = static_cast<LayerModel *>(child(layerIndex));
        if (lyr->text() == name) {
            return lyr;
        }
        else {
            LayerModel *child = lyr->findLayerByName(name);
            if (child)
                return child;
        }
    }
    return 0;
}

void LayerModel::moveLayer(int newPosition)
{
   // QStandardItem *item = (QStandardItem*)(this)->parent();

}

QString LayerModel::url() const
{
    return "";
}

/*QQmlListProperty<LayerModel> LayerModel::layersPrivate()
{
    return QQmlListProperty<LayerModel>(this, children());
}*/

bool Ilwis::Ui::LayerModel::isDrawable() const
{
	return _isDrawable;
}

bool Ilwis::Ui::LayerModel::isVectorLayer() const
{
	return _isVector;
}

bool Ilwis::Ui::LayerModel::usesColorData() const
{
	return false;
}

bool LayerModel::active() const
{
    return _active;
}

void LayerModel::active(bool yesno)
{
    _active = yesno;
    if (_active) {
        LayerModel *layer = parentLayer();
        if (layer) {
            layer->active(yesno);
        }
    } else {
        LayerModel *parentLyr = parentLayer();
        if (parentLyr && parentLyr->active() ) {
            bool someActive = false;
            for (int layerIndex = 0; layerIndex < parentLyr->rowCount(); ++layerIndex) {
                LayerModel *lyr = static_cast<LayerModel *>(parentLyr->child(layerIndex));
                someActive |= lyr->active();
            }
            if (!someActive) {
                parentLyr->active(yesno);
            }
        }
    }
    add2ChangedProperties("layer", rowCount() > 0);
    fillData();
	emit onActiveChanged();
}

quint32 Ilwis::Ui::LayerModel::meshCount() const
{
	return _meshes.size();
}

quint32 Ilwis::Ui::LayerModel::meshIndex(int n) const
{
	if (n < _meshes.size())
		return _meshes[n];
	return -1;
}

void Ilwis::Ui::LayerModel::addMeshIndex(quint32 idx)
{
	_meshes.push_back(idx);
}

void Ilwis::Ui::LayerModel::clearMeshIndexes()
{
	_meshes = std::vector<quint32>();
}

QStringList Ilwis::Ui::LayerModel::changedProperties() const
{
	QStringList props;
	for (auto item : _changedProperties) {
		props.append(item);
	}
	return props;
}

void LayerModel::removeFromChangedProperties(const QString & prop)
{
	auto iter = std::find(_changedProperties.begin(), _changedProperties.end(), prop);
	if (iter != _changedProperties.end())
		_changedProperties.erase(iter);
}

Ilwis::Ui::LayerModel *LayerModel::layer(int idx)
{
    if (idx < rowCount())
        return static_cast<LayerModel *>(child(idx));
    return 0;
}

void LayerModel::add2ChangedProperties(const QString & prop, bool propagate)
{
	_changedProperties.insert(prop);
	if (propagate) {
        for (int layerIndex = 0; layerIndex < rowCount(); ++layerIndex) {
            LayerModel *lyr = static_cast<LayerModel *>(child(layerIndex));
            if ( lyr)
			    lyr->add2ChangedProperties(prop, propagate);
		}
	}
}

bool LayerModel::updateGeometry() const
{
	return _geometryChanged;
}

void LayerModel::updateGeometry(bool yesno)
{
    if (_geometryChanged != yesno) {
        if (yesno)
            _prepared = ptNONE;
        else
            _prepared |= ptGEOMETRY;
    }
	_geometryChanged = yesno;
}

void LayerModel::updateGeometry(bool yesno, bool propagate)
{
   // if (_geometryChanged != yesno) {
        if (yesno)
            _prepared = ptNONE;
        else
            _prepared |= ptGEOMETRY;
  //  }
    _geometryChanged = yesno;
    if (propagate) {
        for (int layerIndex = 0; layerIndex < rowCount(); ++layerIndex) {
            LayerModel *lyr = static_cast<LayerModel *>(child(layerIndex));
            lyr->updateGeometry(yesno, propagate);
        }
    }
}

bool LayerModel::isValid() const
{
	return _isValid;
}

bool LayerModel::prepare(int) {
	return true;
}

bool LayerModel::isPrepared(quint32 type) const
{
	return hasType(_prepared, type);
}

void Ilwis::Ui::LayerModel::setActiveAttribute(int idx)
{
    // needs overrule in relevant derivatives
}

void LayerModel::fillAttributes()
{

}

void LayerModel::fillData() {

    QVariantMap vmap;
    vmap["name"] = text();
    vmap["icon"] = icon();
    vmap["readonly"] = isEditable();
    vmap["parenthasfixedstructure"] = parentLayer() ? parentLayer()->hasFixedStructure() : false;
    vmap["fixedstructure"] = hasFixedStructure();
    vmap["nodeid"] = nodeId();
    vmap["active"] = active();
    setData(vmap);
}

QQmlListProperty<LayerModel> LayerModel::childLayersPrivate()
{
    _childeren = QList<LayerModel *>();
    for (int layerIndex = 0; layerIndex < rowCount(); ++layerIndex) {
        LayerModel *lyr = static_cast<LayerModel *>(child(layerIndex));
        _childeren.push_back(lyr);
    }
    return QQmlListProperty<LayerModel>(this, _childeren);
}

bool LayerModel::isSupportLayer() const
{
	return _isSupportLayer;
}

quint32 LayerModel::modelId() const
{
    return _modelId;
}

bool LayerModel::renderReady() {
    return true; // Dummy implementation, because this class can't be abstract
}

void LayerModel::renderReady(bool yesno) {
    // Dummy implementation, because this class can't be abstract
}

bool LayerModel::canUse(quint64 id)  {
	return false;
}

QVariant LayerModel::coord2value(const Coordinate &c, const QString &attrname) const {
	return QVariant();
}

QColor LayerModel::color(const IRepresentation &rpr, const QString& propName, double value, LayerModel::ColorValueMeaning cvm) {
	return QColor();
}