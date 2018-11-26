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

#ifndef OPENGLHELPER_H
#define OPENGLHELPER_H

namespace geos{
namespace geom{
    class Geometry;
}
}

namespace Ilwis{
typedef std::unique_ptr<geos::geom::Geometry> UPGeometry;
class CoordinateSystem;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;

namespace Ui {


class OpenGLHelper
{
public:
    OpenGLHelper();

    static quint32 getVertices(const ICoordinateSystem& csyRoot,
                               const ICoordinateSystem& csyGeom,
                               const UPGeometry& geometry,
                               Raw objectid,
                               QVector<QVector3D> &points,
                               QVector<QVector3D> &normals,
                               std::vector<VertexIndex>& indices,
                               quint32 &boundaryIndex);
private:
    static void getPolygonVertices(const ICoordinateSystem& csyRoot,
                                   const ICoordinateSystem& csyGeom,
                                   const Ilwis::UPGeometry &geometry,
                                   Raw objectid,
                                   QVector<QVector3D> &points,
                                   QVector<QVector3D>& normals,
                                   std::vector<VertexIndex> &indices);
    static void getLineVertices(const ICoordinateSystem &csyRoot, const ICoordinateSystem& csyGeom,
                                const Ilwis::UPGeometry &geometry,
                                Raw objectid, QVector<QVector3D> &points,
                                QVector<QVector3D>& normals,
                                std::vector<VertexIndex> &indices);
    static void getPointVertices(const ICoordinateSystem& csyRoot,
                                 const ICoordinateSystem& csyGeom,
                                 const UPGeometry &geometry,
                                 Raw objectid,
                                 QVector<QVector3D> &points,
                                 QVector<QVector3D>& normals,
                                 std::vector<VertexIndex> &indices);

    //static IlwisTesselator _tesselator;
};
}
}

#endif // OPENGLHELPER_H
