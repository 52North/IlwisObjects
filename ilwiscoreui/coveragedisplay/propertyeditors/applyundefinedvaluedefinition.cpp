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
#include "raster.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "applyundefinedvaluedefinition.h"

using namespace Ilwis;
using namespace Ui;



ApplyUndefinedValueDefinition::ApplyUndefinedValueDefinition()
{

}

ApplyUndefinedValueDefinition::ApplyUndefinedValueDefinition(VisualAttribute *p) :
    VisualPropertyEditor(p, "applyundefinedvalue",TR("Set Undefined value"),QUrl("UndefinedValueSetter.qml"))
{

}

bool ApplyUndefinedValueDefinition::canUse(const IIlwisObject &obj, const QString &name) const
{
    if (!obj.isValid())
        return false;
    if ( obj->ilwisType() == itRASTER)   {
        Ilwis::IRasterCoverage raster = obj.as<RasterCoverage>();
        return hasType(raster->datadef().domain()->ilwisType(), itNUMERICDOMAIN);
    }
    return false;
}

VisualPropertyEditor *ApplyUndefinedValueDefinition::create(VisualAttribute *p)
{
    return new ApplyUndefinedValueDefinition(p);
}

double ApplyUndefinedValueDefinition::undefinedvalue() const
{
    if ( vpmodel()->layer())
        return vpmodel()->layer()->vproperty("undefinedvalue").toFloat();
    return rUNDEF;
}

void ApplyUndefinedValueDefinition::undefinedvalue(double tr)
{
    if (!vpmodel()->layer())
        return;

    vpmodel()->layer()->vproperty("undefinedvalue", tr);
   // attribute()->layer()->unprepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER);
   // attribute()->layer()->prepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER, IOOptions());
    vpmodel()->layer()->redraw();

}

