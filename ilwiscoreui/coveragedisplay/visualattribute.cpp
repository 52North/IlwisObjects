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
#include <QColor>
#include "ilwisdata.h"
#include "domain.h"
#include "colorlookup.h"
#include "range.h"
#include "itemrange.h"
#include "colorrange.h"
#include "numericrange.h"
#include "representation.h"
#include "raster.h"
#include "models/uicontextmodel.h"
#include "visualpropertyeditor.h"
#include "containerstatistics.h"
#include "visualattribute.h"

using namespace Ilwis;
using namespace Ui;

const QString VisualAttribute::LAYERATTRIBUTE="ILWIS_SINGLE_VALUE_LAYER_ATTRIBUTE";
const QString VisualAttribute::LAYER_ONLY = "ilwis_layer_only_attribute";
const QString VisualAttribute::GLOBAL_ONLY = "ilwis_global_layer_only_attribute";

VisualAttribute::VisualAttribute()
{
}

VisualAttribute::VisualAttribute(LayerModel *layer, const DataDefinition &def, const QString &attrName, const IRepresentation &rpr):
    AttributeModel(def,attrName,layer),
    _layer(layer)
{
    if ( def.isValid()){
        if ( rpr.isValid() && rpr->isCompatible(def.domain()))
            _representation = rpr;
        else
            _representation = Representation::defaultRepresentation(def.domain());
        if (_representation->isSystemObject())
            _representation = _representation->copyWith(def.domain());
        else
            _representation->domain(def.domain());
		if (hasType(def.domain()->ilwisType(), itNUMERICDOMAIN)) {
			_actualRange = *(def.range()->as<NumericRange>());
		}
    }
}


VisualAttribute::VisualAttribute(const VisualAttribute &avp)
{
    _representation = avp._representation;
    _actualRange = avp._actualRange;
    _stretchRange = avp._stretchRange;
    _datadef = avp._datadef;
}

IRepresentation VisualAttribute::representation() const
{
    return _representation;
}

void VisualAttribute::representation(const IRepresentation &rpr)
{
	if (_representation.isValid() && rpr.isValid() && rpr->id() == _representation->id())
		return;

    if ( !datadefinition().isValid())
        return;
    if ( !datadefinition().domain().isValid() || !rpr.isValid())
        return;
    if ( rpr->isCompatible(datadefinition().domain())){
        _representation = rpr;
        for (VisualPropertyEditor *editor : _vproperties) {
            editor->representationChanged(rpr);
        }
    }
}

IDomain VisualAttribute::domain() const
{
    if ( datadefinition().isValid())
        return datadefinition().domain();

    return IDomain();
}

void VisualAttribute::domain(const IDomain &dom)
{
    if ( _representation.isValid() && !_representation->isCompatible(dom)){
        _representation = IRepresentation();
    }
    _datadef = DataDefinition(dom);
}

NumericRange VisualAttribute::stretchRange(bool useActual) const
{
	if (useActual)
		return _stretchRange;

    if ( _stretchRange.isValid())
        return _stretchRange;
    return _actualRange;
}

void VisualAttribute::stretchRange(const NumericRange &rng)
{
    _stretchRange = rng;
	if (!_actualRange.contains(&rng)) {
		_actualRange = rng;
	}
}

quint32 tovalue2(const QString& name){
    short asnum = 0;
    for(auto c : name){
        asnum += c.unicode();
    }
    return asnum * name.size();
}

std::vector<QColor> VisualAttribute::colors(int size) const{
    if ( _representation.isValid() &&  _representation->colors())
        return _representation->colors()->values2colors(_actualRange, _stretchRange,size) ;
    return std::vector<QColor>();
}

std::vector<QColor> VisualAttribute::stretchedColors(int size, NumericRange & displayStretchRange) const{
    if ( _representation.isValid() &&  _representation->colors())
        return _representation->colors()->values2colors(displayStretchRange, _stretchRange,size) ;
    return std::vector<QColor>();
}

LayerModel *VisualAttribute::layer() const
{
    return _layer;
}

bool Ilwis::Ui::VisualAttribute::isAbstractAttribute() const
{
	return _isAbstractAttribute;
}

void VisualAttribute::addVisualPropertyEditor(VisualPropertyEditor *property)
{
    for(VisualPropertyEditor * prop : _vproperties){
        if ( prop->name() == property->attributeName())
            return;
    }
	bool inserted = false;
	for (int i = 0; i < _vproperties.size() && !inserted; ++i) {
		if (property->name() < _vproperties[i]->name()) {
			_vproperties.insert(i, property);
			inserted = true;
		}
	}
	if(!inserted)
		_vproperties.push_back(property);
}

QQmlListProperty<VisualPropertyEditor> VisualAttribute::visualProperties()
{
    return QQmlListProperty<VisualPropertyEditor>(this, _vproperties);
}

VisualPropertyEditor *VisualAttribute::visualProperty(const QString &name) const
{
    for(VisualPropertyEditor * prop : _vproperties){
        if ( prop->name() == name)
            return prop;
    }
    return 0;
}

QColor VisualAttribute::value2color(const QVariant &var) const
{
    if ( !datadefinition().isValid()){ // the range case
        bool ok;
        double d = var.toDouble(&ok);
        if ( ok){
            double f = (d - _actualRange.min())/_actualRange.distance();
            int defColorSize = ColorRangeBase::defaultColorNames().size();
            int index = std::max(0.0,std::min((double)defColorSize-1, f * (defColorSize)));
            return ColorRangeBase::defaultColor(index);
        }
    }
	if (datadefinition().isValid()) {
		double v;
		if (hasType(datadefinition().domain()->valueType(), itCONTINUOUSCOLOR)) {
			return ColorRangeBase::toColor(var.toULongLong(), ColorRangeBase::cmRGBA);
		}
		else if (hasType(datadefinition().domain()->ilwisType(), itNUMERICDOMAIN | itITEMDOMAIN)) {
			v = var.toDouble();
		}
		else {
			v = tovalue2(var.toString());
		}

		return _representation->colors()->value2color(v, _actualRange, _stretchRange);
	}
	return QColor();
}

NumericRange VisualAttribute::actualRange() const
{
    return _actualRange;
}

void VisualAttribute::actualRange(const NumericRange &rng)
{
    _actualRange = rng;
}

//--------------------------------------------------------------------------
LayerAttributeModel::LayerAttributeModel(LayerModel *parentLayer, const IIlwisObject& obj, const DataDefinition &def) :
    VisualAttribute(parentLayer,def, LAYER_WIDE_ATTRIBUTE)
{
   _vproperties = uicontext()->propertyEditors(this, obj, VisualAttribute::LAYER_ONLY);
   _isAbstractAttribute = true;
}

//----------------------------------------------------------------------------
GlobalAttributeModel::GlobalAttributeModel(const QString &label, const QString& associatedEditor, LayerModel *parentLayer) :
    VisualAttribute(parentLayer, DataDefinition(), label),
    _label(label)
{
	_vproperties.append(uicontext()->propertyEditors(this, IIlwisObject(), associatedEditor));
	_isAbstractAttribute = true;
}


