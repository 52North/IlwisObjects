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
#include "coveragelayermodel.h"
#include "rootlayermodel.h"

using namespace Ilwis;
using namespace Ui;

void Ilwis::Ui::CoverageLayerModel::setActiveAttribute(int idx)
{
	if (idx < _visualAttributes.size()) {
		_activeAttribute = _visualAttributes[idx]->attributename();
		add2ChangedProperties("activeattribute",true);
		emit activeAttributeChanged();
	}
}

Ilwis::Ui::CoverageLayerModel::CoverageLayerModel() : LayerModel()
{
}

CoverageLayerModel::CoverageLayerModel( LayerManager *manager, QObject *parent, const QString &name, const QString &desc, const IOOptions& options) :
    LayerModel(manager, parent, name, desc, options)
{
	_layerType = itCOVERAGELAYER;
	_isDrawable = true;
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
	return LayerModel::vproperty(pName);
}

void CoverageLayerModel::vproperty(const QString &attr, const QVariant &value)
{
	LayerModel::vproperty(attr, value);
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
    _coverage = cov;
    if ( _coverage.isValid()){
        _coordConversionNeeded = _coverage->coordinateSystem()->isCompatibleWith(layersManager()->rootLayer()->screenCsy().ptr());
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

    if ( coverage()->coordinateSystem() != layersManager()->rootLayer()->screenCsy()){
        crd = coverage()->coordinateSystem()->coord2coord(layersManager()->rootLayer()->screenCsy(), crd);
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
            QVariant activeAttr =  vmap[visualVPropertyByIndex(activeAttIndex)];
            txt = value2string(activeAttr, visualVPropertyByIndex(activeAttIndex));
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

