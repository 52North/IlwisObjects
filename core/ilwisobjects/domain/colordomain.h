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

#ifndef COLORDOMAIN_H
#define COLORDOMAIN_H

#include "domain.h"
#include "colorrange.h"

namespace Ilwis {
class KERNELSHARED_EXPORT ColorDomain : public Domain
{
public:
    ColorDomain();
    ColorDomain( const Resource& resource);
    IlwisTypes valueType() const;
    IlwisTypes ilwisType() const;
    QVariant impliedValue(const QVariant& value) const    ;
    Containement contains(const QVariant& value) const;
    bool isCompatibleWith(const Ilwis::IlwisObject *dom, bool strict=false) const;
    void range(Range *colorrange);
    bool isOrdered() const;
    void setParent(const IDomain& dm);

    IlwisObject *clone();
protected:
    QSharedPointer<Range> getRange() const;

private:

    QSharedPointer<Range> _range;
    void copyTo(IlwisObject *obj);
};

typedef IlwisData<ColorDomain> IColorDomain;
}

#endif // COLORDOMAIN_H
