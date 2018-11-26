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

#include <QString>
#include "module.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"

using namespace Ilwis;

ColumnDefinition::ColumnDefinition(bool readOnly) : _readOnly(readOnly), _changed(false)
{
}

ColumnDefinition::ColumnDefinition(const ColumnDefinition &def, quint32 index, bool readOnly) :
    Identity(def.name(), index),
    _readOnly(readOnly),
    _changed(false)
{
    datadef().domain(def.datadef().domain<>());
    if ( !def.datadef().range().isNull())
        datadef().range(def.datadef().range()->clone());

}

ColumnDefinition::ColumnDefinition(const QString &name, const DataDefinition &def, quint32 colindex, bool readOnly) :
    Identity(name, colindex),
    _datadef(def),
    _readOnly(readOnly),
    _changed(false)

{

}

ColumnDefinition::ColumnDefinition(const QString &name, const IDomain &dom, quint32 colindex, bool readOnly) :
    Identity(name, colindex),
    _readOnly(readOnly),
    _changed(false)
{
    datadef().domain(dom);
}

bool ColumnDefinition::isValid() const
{
   return name() != sUNDEF && datadef().domain<>().isValid();
}

QString ColumnDefinition::type() const
{
    return "Column";
}

const DataDefinition &ColumnDefinition::datadef() const
{
    return _datadef;
}

DataDefinition &ColumnDefinition::datadef()
{
    return _datadef;
}

void ColumnDefinition::columnindex(quint64 idx)
{
    setId(idx);
}

quint64 ColumnDefinition::columnindex() const
{
    return id();
}

bool ColumnDefinition::isChanged() const
{
    return _changed;
}

void ColumnDefinition::changed(bool yesno)
{
    _changed = yesno;
}

bool ColumnDefinition::isReadOnly() const
{
    return _readOnly;
}

bool ColumnDefinition::equals(const ColumnDefinition &def) const
{
    if ( def.name() != name())
        return false;
    if ( datadef().domain() != def.datadef().domain())
        return false;
    return true;
}


//---------------------------------------------------------

bool Ilwis::operator==(const ColumnDefinition &def1, const ColumnDefinition &def2)
{
    return def1.datadef() == def2.datadef();
}

bool Ilwis::operator!=(const ColumnDefinition &def1, const ColumnDefinition &def2)
{
    return !operator==(def1, def2);
}
