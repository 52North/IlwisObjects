#pragma once

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



#include "thematicitem.h"

namespace Ilwis {

class  TimeIntervalItemRange;

class KERNELSHARED_EXPORT TimeIntervalItem : public ThematicItem
{
public:
    friend class TimeInterval;

    /**
     * Constructor for an empty TimeIntervalItem
     */
    TimeIntervalItem();

    /**
     * Constructor for a TimeIntervalItem with range vr.<br>
     * vr must be valid. vr will be set as the range of this item.
     *
     * @param vr the NumericRange of this item
     */
    TimeIntervalItem(const QString& label, const TimeInterval &vr);

    virtual ~TimeIntervalItem() {}

    //@override
    bool isValid() const;
     bool isOrdered() const;

    //@override
    DomainItem *clone() const;

    //@override
    IlwisTypes valueType() const;

    /**
     * Query for the range of this TimeIntervalItem.<br>
     * Returns the numeric range of this item, if the range is not set it will return null
     *
     * @return the NumericRange of this
     */
    TimeInterval range() const;
	TimeInterval& rangeRef() ;

    /**
     * Sets a new NumericRange on this TimeIntervalItem.<br>
     * The new range must be valid.
     * @param valuerange the new range
     */
    void range(const TimeInterval &valuerange);

    /**
     * Clears the item range on this item.
     *\sa NumericRange
     */
    void clear();

    /**
     * Creates a empty new NumericItemRange and returns this
     * \sa NumericItemRange
     * @return a empty new NumericItemRange
     */
    static Ilwis::TimeInterval *createRange();


    //@override
    static IlwisTypes valueTypeS();

private:
	TimeInterval _valuerange;
};

struct CompareTimeItem {
    bool operator()(const TimeIntervalItem& it1, const TimeIntervalItem& it2)     {
        return it1.range() < it2.range();
    }
};

typedef QSharedPointer<TimeIntervalItem> SPTimeIntervalItem;

}

