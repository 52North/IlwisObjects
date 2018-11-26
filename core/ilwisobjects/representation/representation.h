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

#ifndef REPRESENTATION_H
#define REPRESENTATION_H

#include "colorlookup.h"
#include "shapelookup.h"

namespace Ilwis {

class ShapeLookup;
typedef std::unique_ptr<ColorLookUp> UPColorLookUp;
typedef std::unique_ptr<ShapeLookUp> UPShapeLookUp;

class Domain;
typedef IlwisData<Domain> IDomain;

class KERNELSHARED_EXPORT Representation : public IlwisObject
{
public:
    Representation();
    Representation(const Resource& resource);

    IlwisData<Representation> copyWith(const IDomain& dom) const;
    const UPColorLookUp& colors() const;
    void colors(ColorLookUp *lookup);
    void shapes(ShapeLookUp* lookup);
    const UPShapeLookUp& shapes() const;

    IDomain domain() const;
    void domain(const IDomain& domain);

    bool isCompatible(const IDomain& otherDomain);
    IlwisTypes ilwisType() const;

    static IlwisData<Representation> defaultRepresentation(const Ilwis::IDomain &dom);

    bool isValid() const;

private:
    UPColorLookUp _colors;
    UPShapeLookUp _shapes;
    IDomain _domain;
};

typedef IlwisData<Representation> IRepresentation;
}

Q_DECLARE_METATYPE(Ilwis::IRepresentation)

#endif // REPRESENTATION_H
