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
#include "datadefinition.h"
#include "columndefinition.h"
#include "representation.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "coveragelayermodel.h"
#include "raster.h"
#include "colorlookup.h"
#include "continuouscolorlookup.h"
#include "numericrepresentationsetter.h"
#include "mathhelper.h"


using namespace Ilwis;
using namespace Ui;

NumericRepresentationSetter::NumericRepresentationSetter(VisualAttribute *p) :
        VisualPropertyEditor(p,"numericrepresentationsetter",TR("Legend"),QUrl("NumericRepresentation.qml"))
{

}

NumericRepresentationSetter::NumericRepresentationSetter()
{
	
}

bool NumericRepresentationSetter::canUse(const IIlwisObject &obj, const QString& name) const
{
	if (name == PIXELVALUE && obj->ilwisType() == itRASTER) {
		IRasterCoverage raster = obj.as<RasterCoverage>();
		return (hasType(raster->datadef().domain()->ilwisType(), itNUMERICDOMAIN));
	}
	return false;
}

bool NumericRepresentationSetter::canUse(const IIlwisObject &obj, const DataDefinition &def) const 
{
	if (def.isValid()) {
		auto ok = hasType(def.domain()->ilwisType(), itNUMERICDOMAIN);
		if (ok)
			return true;
	}
    return false;
}

VisualPropertyEditor *NumericRepresentationSetter::create(VisualAttribute *p)
{
    return new NumericRepresentationSetter(p);
}

void NumericRepresentationSetter::fillElements() {
	if (vpmodel()->layer()) {
		//QVariant actAttribute = vpmodel()->layer()->vproperty("activevisualattribute");
		//if (!actAttribute.isValid())
		//	return;
		QString atrName = attributeName();
		auto var = vpmodel()->layer()->vproperty("visualattribute|actualrange|" + atrName);
		NumericRange numrange = var.value<NumericRange>();
		if (!numrange.isValid())
			return;

		RepresentationElementModel::createNumericElementsList(numrange, this, _rprElements);

		//emit rprNameChanged();
		emit itemsChanged();
	}
}
void NumericRepresentationSetter::prepare( const IIlwisObject &obj, const DataDefinition &cdef)
{
    VisualPropertyEditor::prepare( obj, cdef);

	fillElements();
 
}

QColor NumericRepresentationSetter::color(const QString& property, double frac)
{
    if ( vpmodel()->layer()){
		auto *layer = vpmodel()->layer();
		auto *clayer = layer->as<CoverageLayerModel>();
		auto rpr = vpmodel()->representation();
        return clayer->color(rpr, property, frac,LayerModel::cvmFRACTION) ;
    }
    return QColor();
}

bool NumericRepresentationSetter::canUse(const QString &id) const
{
    try {
        bool ok;
        quint64 objid = id.toULongLong(&ok);
        if ( !ok)
            return false;
        Resource resource = mastercatalog()->id2Resource(objid);
        if ( !resource.isValid())
            return false;
        if ( resource.ilwisType() != itREPRESENTATION)
            return false;
        IRepresentation representation(resource);
        return hasType(representation->domain()->ilwisType(), itNUMERICDOMAIN);

    } catch(const ErrorObject& ){

    }
    return false;

}

void NumericRepresentationSetter::representationChanged(const IRepresentation &rpr)
{
	VisualAttribute *actAttribute = vpmodel()->layer()->activeAttribute();
	if (!actAttribute)
		return;
	fillElements();
	vpmodel()->layer()->prepare(LayerModel::ptRENDER);
	vpmodel()->layer()->redraw();
}

QQmlListProperty<RepresentationElementModel> NumericRepresentationSetter::representationElements()
{
	//fillElements();
    return  QQmlListProperty<RepresentationElementModel>(this, _rprElements);
}

void NumericRepresentationSetter::addException(double value, double bandWdith, const QColor& clr) {
	auto rpr = vpmodel()->representation();
	ContinuousColorLookup *lookup = static_cast<ContinuousColorLookup *>(rpr->colors().get());
	if (value == -1) {
		lookup->addException(NumericRange(), clr, true);
		vpmodel()->layer()->prepare(LayerModel::ptRENDER);
	}
	else {
		double rmin = std::max(0.0, value - bandWdith / 2);
		double rmax = std::min(1.0, value + bandWdith / 2);
		NumericRange nr(rmin, rmax);
		lookup->addException(nr, clr, true);
	}
	vpmodel()->layer()->prepare(LayerModel::ptRENDER);


}



