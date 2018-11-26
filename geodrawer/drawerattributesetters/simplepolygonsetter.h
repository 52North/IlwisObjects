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

#ifndef SIMPLEPOLYGONSETTER_H
#define SIMPLEPOLYGONSETTER_H

#include "basespatialattributesetter.h"

namespace Ilwis {
namespace Ui{


class SimplePolygonSetter : public BaseSpatialAttributeSetter
{
public:
    SimplePolygonSetter(const LayerManager *manager);

	void getVertices(const geos::geom::Geometry *subgeom, std::vector<qreal>& vertices, std::vector<int>& indices) const;

    void getColors(const VisualAttribute& attr,
                            const QVariant& value,const QColor& defaultColor,int start,
							std::vector<qreal>& colors) const;

};
}
}

#endif // SIMPLEPOLYGONSETTER_H
