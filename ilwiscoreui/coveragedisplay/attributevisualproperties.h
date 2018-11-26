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

#ifndef ATTRIBUTEVISUALPROPERTIES_H
#define ATTRIBUTEVISUALPROPERTIES_H

#include "ilwiscoreui_global.h"

namespace Ilwis {

class Representation;
class Domain;
class RasterCoverage;

typedef IlwisData<Representation> IRepresentation;
typedef IlwisData<Domain> IDomain;
typedef IlwisData<RasterCoverage> IRasterCoverage;

namespace Geodrawer{

class ILWISCOREUISHARED_EXPORT VisualAttribute
{
public:
    VisualAttribute();
    VisualAttribute(const NumericRange& rng);
    VisualAttribute(const IDomain& dom, int colindex = iUNDEF, const Ilwis::IRepresentation &rpr = IRepresentation());
    VisualAttribute(const VisualAttribute& avp);

    IRepresentation representation() const;
    void representation(const IRepresentation& rpr);
    IDomain domain() const;
    void domain(const IDomain& dom);
    NumericRange stretchRange() const;
    void stretchRange(const NumericRange& rng);
    QColor value2color(const QVariant& var) const;
    NumericRange actualRange() const;
    void actualRange(const NumericRange& rng);
    quint32 columnIndex() const;
    void setColumnIndex(quint32 index);

    const static QString LAYERATTRIBUTE;

    std::vector<QColor> colors(int size=256) const;
private:
    IRepresentation _representation;
    NumericRange _stretchRange;
    NumericRange _actualRange;
    IDomain _domain;
    IRasterCoverage _raster;
    quint32 _columnIndex=iUNDEF; //optional indicator which column/attribute is used for these properties
};
}
}

#endif // ATTRIBUTEVISUALPROPERTIES_H
