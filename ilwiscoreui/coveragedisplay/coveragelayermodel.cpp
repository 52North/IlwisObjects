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
#include "coverage.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "layermanager.h"
#include "representation.h"
#include "visualpropertyeditor.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "layerinfoitem.h"
#include "modelregistry.h"
#include "coveragelayermodel.h"
#include "rootlayermodel.h"

using namespace Ilwis;
using namespace Ui;

void Ilwis::Ui::CoverageLayerModel::setActiveAttribute(int idx)
{
	if (idx < _visualAttributes.size()) {
		_activeAttribute = _visualAttributes[idx]->attributename();
		add2ChangedProperties("buffers",true);
		emit activeAttributeChanged();
	}
}

void Ilwis::Ui::CoverageLayerModel::linkMessage(const QVariantMap & parms)
{
}

Ilwis::Ui::CoverageLayerModel::CoverageLayerModel() : LayerModel()
{
}

CoverageLayerModel::CoverageLayerModel( LayerManager *manager, QStandardItem *parent, const QString &name, const QString &desc, const IOOptions& options) :
    LayerModel(manager, parent, name, desc, options)
{
	_layerType = itCOVERAGELAYER;
	_isDrawable = true;
 
}

Ilwis::Ui::CoverageLayerModel::~CoverageLayerModel()
{
    modelregistry()->unRegisterModel(modelId());
}

QString CoverageLayerModel::activeAttributeName() const
{
    return _activeAttribute;
}

void CoverageLayerModel::activeAttributeName(const QString& pName)
{
    try{
        if ( pName == _activeAttribute)
            return;
        if ( visualAttribute(pName) != 0)
            _activeAttribute = pName;
        emit activeAttributeChanged();
    } catch(const ErrorObject& ){

    }
}

VisualAttribute * CoverageLayerModel::activeAttribute()
{
	return visualAttribute(_activeAttribute);
}

QVariant CoverageLayerModel::vproperty(const QString &pName) const
{
    QVariant v = LayerModel::vproperty(pName);
    if (v.isValid())
        return v;
	if (pName == "activevisualattribute") {
		return  _activeAttribute;
	}
    if (pName == "envelope") {
        Envelope env = coverage()->envelope();
        return env.toMap();
    }
    return QVariant();
}

void CoverageLayerModel::vproperty(const QString &attr, const QVariant &value)
{
	LayerModel::vproperty(attr, value);
	if (attr == "activeattribute") {
		activeAttributeName(value.toString());
	}
}

bool CoverageLayerModel::showInfo() const
{
    return _showInfo;
}

void CoverageLayerModel::showInfo(bool yesno)
{
    _showInfo = yesno;
}

Ilwis::ICoverage CoverageLayerModel::coverage() const
{
    return _coverage;
}

void CoverageLayerModel::coverage(const ICoverage &cov)
{
    _coverage = cov;
    fillAttributes();
    if ( _coverage.isValid()){
        _coordConversionNeeded = _coverage->coordinateSystem()->isCompatibleWith(layerManager()->rootLayer()->screenCsy().ptr());
        modelregistry()->registerModel(modelId(), TypeHelper::type2name(_coverage->ilwisType()), this);
        if (layerManager()->layerListName() == sUNDEF)
            layerManager()->setLayerListName(_coverage->name());
      }
}

QString CoverageLayerModel::url() const
{

    if ( _coverage.isValid())
        return _coverage->resource().url().toString();
    return QString();
}

void CoverageLayerModel::addVisualAttribute(const QString &attrName, const QString& propertyName, VisualPropertyEditor *newProp)
{
    VisualAttribute *attr = visualAttribute(attrName);
    if ( attr != 0){
            attr->addVisualPropertyEditor(newProp);
    }
}

bool CoverageLayerModel::needsCoordConversion() const
{
    return _coordConversionNeeded;
}

VisualPropertyEditor*CoverageLayerModel::visualProperty(const QString &attrName, const QString& propertyName)
{
    VisualAttribute *attr = visualAttribute(attrName);
    if ( attr != 0){
        attr->visualProperty(propertyName);
    }
    return 0;
}

QString Ilwis::Ui::CoverageLayerModel::ilwisIdPrivate() const
{
    if (_coverage.isValid())
        return QString::number(_coverage->id());
    return "";
}

QQmlListProperty<VisualPropertyEditor> CoverageLayerModel::vproperties(const QString &attrName)
{
    VisualAttribute *attr = visualAttribute(attrName);
    if ( attr){
        return attr->visualProperties();
    }
    return QQmlListProperty<VisualPropertyEditor>();
}

QColor CoverageLayerModel::color(const IRepresentation &rpr, const QString& propName, double value, LayerModel::ColorValueMeaning cvm)
{
    if ( _activeAttribute != sUNDEF){
        VisualAttribute* attr = visualAttribute(_activeAttribute);
        if (!attr)
            return QColor();

        if ( cvm ==LayerModel::cvmFRACTION){
            NumericRange numrange = attr->stretchRange();
            if ( !numrange.isValid()){
                numrange = attr->actualRange();
            }
            value = numrange.min() + numrange.distance() * value;

        }
        QColor clr = attr->value2color(value);
        return clr;
    }
    return QColor();
}

QString CoverageLayerModel::value2string(const QVariant &value, const QString &attrName) const
{
    return sUNDEF;
}

QString CoverageLayerModel::layerData(const Coordinate &crdIn, const QString &attrName, QVariantList &items) const
{
    Coordinate crd = crdIn;

    if ( coverage()->coordinateSystem() != layerManager()->rootLayer()->screenCsy()){
        crd = coverage()->coordinateSystem()->coord2coord(layerManager()->rootLayer()->screenCsy(), crd);
    }

    QVariantMap item;
    item["name"] = "Layer";
    item["value"] = coverage()->name();
    item["icon"] = ResourceModel::icon(coverage()->resource());
    item["header"] = true;
    items.push_back(item);

    std::vector<QString> texts;

    QVariant value = coverage()->coord2value(crd,attrName);
    if ( value.isValid()){
        QString txt;
        if ( attrName != "")
            txt = value2string(value,attrName);
        else {
            QVariantMap vmap = value.value<QVariantMap>();
            int activeAttIndex = activeVProperty();
            if ( activeAttIndex == 0){ // this is the pseudo attribute representing the whole layer, not wanted here
                activeAttIndex = 1;
            }
            //txt = vmap[_activeAttribute].toString();
            if (_activeAttribute == LAYER_WIDE_ATTRIBUTE) {
                if (vmap.contains(PIXELVALUE)) {
                    txt = value2string(vmap[PIXELVALUE], "");
                }
            }else
                txt = value2string(vmap[_activeAttribute], _activeAttribute);
            auto end = vmap.end();
            for(auto item = vmap.begin(); item != end; ++item){
                if ( item.value().isValid()){
                    QString attTxt = value2string(item.value(), item.key());
                    QVariantMap vitem;
                    vitem["name"] = item.key();
                    vitem["value"] = attTxt;
                    items.push_back(vitem);
                }
            }
        }
        if (showInfo())
            texts.push_back(txt);
    }

    QString outtext;
    for(auto txt : texts){
        if ( outtext.size() != 0)
            outtext += "; ";
        outtext += txt;
    }
	QString childTexts = LayerModel::layerData(crdIn, attrName, items);
	if (outtext != "" && childTexts != "")
		outtext += ";";
	outtext += childTexts;

    if ( outtext == "")
        outtext = sUNDEF;


    return outtext;
}

bool Ilwis::Ui::CoverageLayerModel::isCoverageBased() const
{
    return true;
}

QString CoverageLayerModel::v2s(const ColumnDefinition& coldef, const QVariant& value) const
{
    if ( coldef.isValid()){
        if ( coldef.datadef().domain()->ilwisType() == itTEXTDOMAIN)
            return value.toString();

        QVariant impliedValue = coldef.datadef().domain()->impliedValue(value);
        if (coldef.datadef().domain()->valueType() == itNUMERICDOMAIN ) {
            double resolution = coldef.datadef().range().dynamicCast<NumericRange>()->resolution();
            if ( resolution == 0)
                return QString::number(impliedValue.toDouble(), 'f', 3);
            if ( resolution == 1){
                return QString::number(impliedValue.toLongLong());
            }
        }
        return impliedValue.toString();
    }
    if ( value.toDouble() == rUNDEF)
        return sUNDEF;
    return value.toString();
}

bool CoverageLayerModel::supportsLinkType(const QString& type) const
{
    return  type.toLower() ==  "selectionbyraw" || type.toLower() == "zoom";
}

QVariantList CoverageLayerModel::linkProperties() const
{
    QVariantList result;
    return result;
}

void CoverageLayerModel::sendLink(const QVariantMap & parameters)
{
    emit linkSendMessage(parameters);
}

QString Ilwis::Ui::CoverageLayerModel::mainPanelUrl() 
{
    return "visualization/MapPanel.qml";
}

void CoverageLayerModel::linkAcceptMessage(const QVariantMap& parameters) {
    qDebug() << "gets message";
}


