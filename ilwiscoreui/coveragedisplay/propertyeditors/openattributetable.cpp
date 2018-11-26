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

#include "coverage.h"
#include "table.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "coveragelayermodel.h"
#include "openattributetable.h"

using namespace Ilwis;
using namespace Ui;

OpenAttributeTable::OpenAttributeTable()
{

}

OpenAttributeTable::OpenAttributeTable(VisualAttribute *p) : VisualPropertyEditor(p,"viewattributes",TR("View Attributes"),QUrl("ViewAttributes.qml"))
{
}

bool OpenAttributeTable::canUse(const IIlwisObject &obj, const QString &name) const
{
    if (!obj.isValid())
        return false;
    if(!hasType(obj->ilwisType(), itCOVERAGE))
        return false;
    Ilwis::ICoverage cov = obj.as<Coverage>();

    return name == VisualAttribute::LAYER_ONLY && cov->hasAttributes() && !vpmodel()->layer()->isSupportLayer();
}

QStringList OpenAttributeTable::attributeTable() const
{
    QStringList result;
    if ( vpmodel()->layer()){
		auto *lyr = vpmodel()->layer();
        ICoverage coverage = lyr->as<CoverageLayerModel>()->coverage();
        if(coverage->hasAttributes() ){
            Resource res;
            if ( coverage->ilwisType() == itFEATURE)
                res =coverage->resource();
            else
                res = coverage->attributeTable()->resource();
            result.append(QString::number(res.id()));
            result.append(res.url().toString());

        }
    }
    return result;
}

VisualPropertyEditor *OpenAttributeTable::create(VisualAttribute *p)
{
    return new OpenAttributeTable(p);
}
