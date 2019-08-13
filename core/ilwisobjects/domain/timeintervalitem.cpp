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
#include "juliantime.h"
#include "timeintervalitem.h"

using namespace Ilwis;

TimeIntervalItem::TimeIntervalItem()
{
}

TimeIntervalItem::TimeIntervalItem(const QString& label, const TimeInterval &vr) : ThematicItem({label}), _valuerange(vr)
{
}

bool TimeIntervalItem::isValid() const
{
    return _valuerange.isValid();
}

bool TimeIntervalItem::isOrdered() const
{
    return true;
}

DomainItem *TimeIntervalItem::clone() const
{
    TimeIntervalItem *item =  new TimeIntervalItem(name(), _valuerange);
    item->code(code());
    item->description(description());
    return item;
}

IlwisTypes TimeIntervalItem::valueTypeS()
{
    return itNUMERICITEM;
}

IlwisTypes TimeIntervalItem::valueType() const
{
    return valueTypeS();
}

TimeInterval TimeIntervalItem::range() const
{
    return _valuerange;
}

TimeInterval& TimeIntervalItem::rangeRef()
{
	return _valuerange;
}

void TimeIntervalItem::range(const TimeInterval &valuerange)
{
    _valuerange = valuerange;
}

void TimeIntervalItem::clear()
{
    _valuerange.clear();
}

Ilwis::TimeInterval *TimeIntervalItem::createRange(){
    return new TimeInterval();
}




