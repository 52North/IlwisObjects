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

#ifndef SIMPLELINESETTER_H
#define SIMPLELINESETTER_H

#include "basespatialattributesetter.h"

namespace Ilwis {
namespace Ui{

class SimpleLineSetter : public BaseSpatialAttributeSetter
{
public:
    SimpleLineSetter(const LayerManager *manager);
    ~SimpleLineSetter();

	//void getGeometryVertices(const geos::geom::Geometry * geom, Vertices& vertices, VertexIndices& indices) const;

	void getVertices(const geos::geom::Geometry *subgeom, Vertices& vertices, VertexIndices& indices) const;

    void getColors(const VisualAttribute& attr,
                            const QVariant& value,const QColor& defaultColor,int start,
							Colors& colors) const;
private : 
	void getVerticesPolygon(const geos::geom::Geometry *geom, Vertices& vertices, VertexIndices& indices) const;
	void getGeometryVertices(const geos::geom::Geometry * geom, Vertices& vertices, VertexIndices& indices) const;

};
}
}

#endif // SIMPLELINESETTER_H
