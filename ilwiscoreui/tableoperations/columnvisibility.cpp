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
#include "table.h"
#include "models/tablemodel.h"
#include "tableoperation.h"
#include "columnvisibility.h"

using namespace Ilwis;
using namespace Ui;

ColumnVisibility::ColumnVisibility() : TableOperation("Column Visibility", QUrl("ColumnVisibility.qml"),TR("Sets the visibility of columns"))
{

}

bool ColumnVisibility::canUse(TableModel *tblModel, const QVariantMap& parameter) const
{
    bool ok = parameter["tableonly"].toBool();
    return ok;
}


void ColumnVisibility::execute(const QVariantMap& parameters )
{
    bool ok;
    quint32 colIndex = parameters["columnindex"].toInt(&ok);

    tableModel()->update();
}

TableOperation *ColumnVisibility::create()
{
    return new ColumnVisibility();
}

