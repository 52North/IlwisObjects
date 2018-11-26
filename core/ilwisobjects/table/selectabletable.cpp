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
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "logicalexpressionparser.h"
#include "tableselector.h"

#include "selectabletable.h"

using namespace Ilwis;

SelectableTable::SelectableTable() : Table()
{
}

SelectableTable::SelectableTable(const Resource& resource) : Table(resource)
{
}

SelectableTable::~SelectableTable()
{
}


std::vector<quint32> SelectableTable::select(const QString &conditions) const
{
    return TableSelector::select(this, conditions);
}

QString SelectableTable::primaryKey() const {
    return "?";
}


