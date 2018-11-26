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

#ifndef VALUEDOMAINDATA_H
#define VALUEDOMAINDATA_H

#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT ValueDomain : public Domain
{
public:
    ValueDomain();
    ValueDomain(const Resource& res);

    const Ilwis::PRange &range() const;
    void setRange(Range *vr);
    QString value(double v) const;

    virtual void debug();

    IlwisTypes valueType() const;
private:
    long significantDigits(double m1);

    PRange _range;

};

typedef IlwisData<ValueDomain> IValueDomain;
}

#endif // VALUEDOMAINDATA_H
