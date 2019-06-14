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

#ifndef GEOMETRIES_H
#define GEOMETRIES_H

#include <type_traits>
#include "geos/geom/Coordinate.h"
#include "ilwis.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"

typedef std::vector<std::vector<quint16>> VertexIndices;
typedef std::vector<std::vector<qreal>> Vertices;
typedef std::vector<std::vector<qreal>> Colors;

#define MAX_INDICES_BUFFER 0xffff



#endif // GEOMETRIES_H
