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
#include "numericrepresentationsetter.h"
#include "mathhelper.h"


using namespace Ilwis;
using namespace Ui;

NumericRepresentationSetter::NumericRepresentationSetter(VisualAttribute *p) :
        VisualPropertyEditor(p,"numericrepresentationsetter",TR("Representation"),QUrl("NumericRepresentation.qml"))
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
    if ( def.isValid())
        return hasType(def.domain()->ilwisType(), itNUMERICDOMAIN);
    return false;
}

VisualPropertyEditor *NumericRepresentationSetter::create(VisualAttribute *p)
{
    return new NumericRepresentationSetter(p);
}

void NumericRepresentationSetter::fillElements() {
	if (vpmodel()->layer()) {
		QVariant actAttribute = vpmodel()->layer()->vproperty("activevisualattribute");
		if (!actAttribute.isValid())
			return;
		auto var = vpmodel()->layer()->vproperty("visualattribute|stretchrange|" + actAttribute.toString());
		NumericRange numrange = var.value<NumericRange>();
		if (!numrange.isValid())
			return;

		NumericRange roundedRange = MathHelper::roundRange(numrange.min(), numrange.max());
		double tickValue = roundedRange.min();
		double step = roundedRange.resolution();
		_rprElements = QList<RepresentationElementModel *>();
		while (tickValue <= numrange.max()) {
			_rprElements.push_back(new RepresentationElementModel(QString::number(tickValue), tickValue / numrange.distance(), this));
			tickValue += step;
		}
		if ((tickValue - roundedRange.resolution()) != numrange.max()) {
			if (tickValue > numrange.max())
				tickValue = numrange.max();
			_rprElements.push_back(new RepresentationElementModel(QString::number(numrange.max()), this));
		}

		emit rprNameChanged();
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
        return vpmodel()->layer()->as<CoverageLayerModel>()->color(vpmodel()->representation(), property, frac,LayerModel::cvmFRACTION) ;
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

void NumericRepresentationSetter::setRepresentation(const QString &name)
{
    Resource resource = mastercatalog()->name2Resource(name, itREPRESENTATION)    ;
    if ( !resource.isValid())
        return;
    IRepresentation rpr(resource);
	VisualAttribute *actAttribute = vpmodel()->layer()->activeAttribute();
    if ( !actAttribute)
        return ;
    QVariant var;
    var.setValue<IRepresentation>(rpr);
	//vpmodel()->layer()->vproperty("visualattribute|representation|" + actAttribute->attributename(),var);
	vpmodel()->layer()->vproperty(QString("visualattribute|%1|representation").arg(actAttribute->attributename()),rpr->resource().url().toString());
	fillElements();
	vpmodel()->layer()->add2ChangedProperties("buffers", true);
}

QQmlListProperty<RepresentationElementModel> NumericRepresentationSetter::representationElements()
{
    return  QQmlListProperty<RepresentationElementModel>(this, _rprElements);
}



