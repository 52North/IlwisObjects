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

#ifndef GEOMETRYHELPER_H
#define GEOMETRYHELPER_H

#include <complex>

namespace geos {
namespace geom {
    class Geometry;
}
namespace io{
    class ParseException;
}
}

namespace Ilwis {

class CoordinateSystem;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;

class KERNELSHARED_EXPORT GeometryHelper{
    public:
        static QString toWKT(const geos::geom::Geometry* geom);
        static geos::geom::Geometry* fromWKT(const QString &wkt, const Ilwis::ICoordinateSystem &csy);

        static IlwisTypes geometryType(const geos::geom::Geometry* geom);

        static void transform(geos::geom::Geometry* geom, const ICoordinateSystem& source, const ICoordinateSystem& target);

        static Ilwis::CoordinateSystem* getCoordinateSystem(geos::geom::Geometry* geom);

        static void setCoordinateSystem(geos::geom::Geometry* geom, Ilwis::CoordinateSystem* csy);

        template<typename PointType> static std::vector<PointType> rotate2d(const PointType &center, const double angle, const std::vector<PointType>& inputPoints)
        {
            typedef std::complex<double> CPoint;
            CPoint about(center.x, center.y);
            std::vector<PointType> outputPoints;
            for(auto p : inputPoints){
                if ( p.isValid()){
                    CPoint outPoint = (CPoint(p.x,p.y) - about) * exp(CPoint(0, angle * M_PI / 180.0)) + about;
                    outputPoints.push_back(PointType(outPoint.real(), outPoint.imag()));
                }else
                    outputPoints.push_back(PointType());
            }
            return outputPoints;
        }
        static std::vector<geos::geom::CoordinateSequence *> geometry2coords(const geos::geom::Geometry *geom);
};

typedef std::unique_ptr<geos::geom::Geometry> UPGeometry;
}

#endif // GEOMETRYHELPER_H
