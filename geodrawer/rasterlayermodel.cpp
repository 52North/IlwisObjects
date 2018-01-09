#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coverage.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "itemrange.h"
#include "colorrange.h"
#include "uicontextmodel.h"
#include "coveragedisplay/layermanager.h"
#include "representation.h"
#include "coveragedisplay/coveragelayermodel.h"
#include "coveragedisplay/layerinfoitem.h"
#include "coveragedisplay/visualattribute.h"
#include "rasterlayermodel.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_LAYERMODEL("rastercoverage",RasterLayerModel)

RasterLayerModel::RasterLayerModel() {

}

RasterLayerModel::RasterLayerModel(LayerManager *manager, QObject *parent, const QString &name, const QString &desc, const IOOptions& options) : CoverageLayerModel(manager, parent,name, desc, options)
{
	_isDrawable = true;
	_isValid = true;
	_icon = "raster.png";
	_layerType = itRASTERLAYER;
}

LayerModel *RasterLayerModel::create(LayerManager *manager, LayerModel *layer, const QString &name, const QString &desc, const IOOptions& options)
{
    return new RasterLayerModel(manager, layer, name, desc, options);
}

bool Ilwis::Ui::RasterLayerModel::usesColorData() const
{
	if (_raster.isValid()) {
		return _raster->datadef().domain()->ilwisType() == itCOLORDOMAIN;
	}
	return false;
}

QVariant RasterLayerModel::vproperty(const QString &attrName) const
{
    return QVariant();
}

void RasterLayerModel::vproperty(const QString &attrName, const QVariant &value)
{

}

QString RasterLayerModel::value2string(const QVariant &value, const QString &attrName) const
{
    if ( attrName != "") {
        IRasterCoverage raster = CoverageLayerModel::coverage().as<RasterCoverage>();
        if ( raster->hasAttributes()){
            ColumnDefinition coldef = raster->attributeTable()->columndefinition(attrName);
            return v2s(coldef, value);
        }
        if ( raster->datadef().domain()->ilwisType() == itCOLORDOMAIN){
            auto clr = ColorRangeBase::toColor(value, ColorRangeBase::cmRGBA);
            return ColorRangeBase::toString(clr,ColorRangeBase::cmRGBA)    ;
        }
        double resolution = raster->datadef().range().dynamicCast<NumericRange>()->resolution();
        if ( resolution == 0)
            return QString::number(value.toDouble(), 'f', 5);
        if ( resolution == 1){
            return QString::number(value.toLongLong());
        }
    }

    if ( value.toDouble() == rUNDEF)
        return sUNDEF;
    //IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();

    return QString::number(value.toDouble(), 'f', 3);

}

QString RasterLayerModel::layerData(const Coordinate &crdIn, const QString &attrName, QVariantList &items) const
{
    QString txt = CoverageLayerModel::layerData(crdIn, attrName, items);
    IRasterCoverage raster = CoverageLayerModel::coverage().as<RasterCoverage>();
    Pixel pix = raster->georeference()->coord2Pixel(crdIn);
    QString pixtxt = QString("%1 %2").arg(pix.x + 1).arg(pix.y + 1);
    QVariantMap vitem;
    vitem["name"] = "Pixel";
    vitem["value"] = pixtxt;
    items.push_back(vitem);

    return txt;
}

void RasterLayerModel::coverage(const ICoverage &cov)
{
    CoverageLayerModel::coverage(cov);
    _raster = CoverageLayerModel::coverage().as<RasterCoverage>();

    fillAttributes();

}

void RasterLayerModel::fillAttributes()
{
    if ( _raster.isValid()){
        _visualAttributes = QList<VisualAttribute *>();
        _visualAttributes.push_back(new LayerAttributeModel(this,_raster, _raster->datadef()));
        _visualAttributes.push_back(new VisualAttribute(this, _raster->datadef(),PIXELVALUE));

        if ( _raster->hasAttributes()){
            for(int i=0; i < _raster->attributeTable()->columnCount(); ++i){
                ColumnDefinition coldef = _raster->attributeTable()->columndefinition(i);
                IlwisTypes valueType = coldef.datadef().domain()->valueType();
                if ( hasType(valueType, itNUMBER|itDOMAINITEM|itSTRING|itCOLOR)){
                    _visualAttributes.push_back(new VisualAttribute(this, coldef.datadef(),coldef.name()));
                }
            }
        }
    }
}

bool Ilwis::Ui::RasterLayerModel::prepare(int prepType)
{
	if (hasType(prepType, LayerModel::ptGEOMETRY) && !isPrepared(LayerModel::ptGEOMETRY)) {
		if (!_raster.isValid()) {
			return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "RasterCoverage", TR("Visualization"));
		}
	}
	return false;
}
