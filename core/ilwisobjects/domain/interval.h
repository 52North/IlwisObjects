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

#ifndef NUMERICITEM_H
#define NUMERICITEM_H

#include "thematicitem.h"

namespace Ilwis {

class  IntervalRange;

class KERNELSHARED_EXPORT Interval : public ThematicItem
{
public:
    friend class IntervalRange;

    /**
     * Constructor for an empty Interval
     */
    Interval();

    /**
     * Constructor for a Interval with range vr.<br>
     * vr must be valid. vr will be set as the range of this item.
     *
     * @param vr the NumericRange of this item
     */
    Interval(const QString& label, const NumericRange &vr);

    virtual ~Interval() {}

    //@override
    bool isValid() const;
     bool isOrdered() const;

    //@override
    DomainItem *clone() const;

    //@override
    IlwisTypes valueType() const;

    /**
     * Query for the range of this Interval.<br>
     * Returns the numeric range of this item, if the range is not set it will return null
     *
     * @return the NumericRange of this
     */
    NumericRange range() const;
	NumericRange& rangeRef() ;

    /**
     * Sets a new NumericRange on this Interval.<br>
     * The new range must be valid.
     * @param valuerange the new range
     */
    void range(const NumericRange &valuerange);

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
    static Ilwis::IntervalRange *createRange();


    //@override
    static IlwisTypes valueTypeS();

private:
    NumericRange _valuerange;
};

struct CompareNumericItem {
    bool operator()(const Interval& it1, const Interval& it2)     {
        return it1.range() < it2.range();
    }
};

typedef QSharedPointer<Interval> SPInterval;

}

#endif // NUMERICITEM_H
