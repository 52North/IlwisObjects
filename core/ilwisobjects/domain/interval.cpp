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
#include <QSharedPointer>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "domain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "numericrange.h"
#include "itemrange.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "interval.h"
#include "intervalrange.h"

using namespace Ilwis;

Interval::Interval()
{
}

Interval::Interval(const QString& label, const NumericRange &vr) : ThematicItem({label}), _valuerange(vr)
{
}

bool Interval::isValid() const
{
    return _valuerange.isValid();
}

bool Interval::isOrdered() const
{
    return true;
}

DomainItem *Interval::clone() const
{
    Interval *item =  new Interval(name(), _valuerange);
    item->code(code());
    item->description(description());
    return item;
}

IlwisTypes Interval::valueTypeS()
{
    return itNUMERICITEM;
}

IlwisTypes Interval::valueType() const
{
    return valueTypeS();
}

NumericRange Interval::range() const
{
    return _valuerange;
}

NumericRange& Interval::rangeRef() 
{
	return _valuerange;
}

void Interval::range(const NumericRange &valuerange)
{
    _valuerange = valuerange;
}

void Interval::clear()
{
    _valuerange.clear();
}

Ilwis::IntervalRange *Interval::createRange(){
    return new IntervalRange();
}




