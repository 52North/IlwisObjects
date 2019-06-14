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

#ifndef ILWISTESSELATOR_H
#define ILWISTESSELATOR_H

#include "tesselator.h"

namespace Ilwis {
namespace Ui{

class IlwisTesselator
{
public:
    IlwisTesselator();
    ~IlwisTesselator();

    void tesselate(const geos::geom::Geometry *geometry, Vertices& vertices, VertexIndices &indices);
private:
   std::vector<std::vector<float>> getContours(const geos::geom::Geometry *geometry);
   void tesselateInternal(const std::vector<std::vector<float> > &contours, Vertices& vertices, VertexIndices& indices);

    TESStesselator *_tessaltor = 0;
    TESSalloc _ma;
    int _allocated = 0;
};
}
}

#endif // ILWISTESSELATOR_H
