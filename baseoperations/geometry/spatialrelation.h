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

#ifndef SPATIALRELATION_H
#define SPATIALRELATION_H

namespace Ilwis {
namespace BaseOperations {

typedef std::function<bool(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2)> SpatialRelation;


class SpatialRelationOperation : public OperationImplementation
{
public:
    SpatialRelationOperation();

    SpatialRelationOperation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable &symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression &expr);
protected:
    static quint64 createMetadata();
    OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    IFeatureCoverage _inputFeatures;
    IFeatureCoverage _relatedFeatures;
    IFeatureCoverage _outputFeatures;
    std::unique_ptr<geos::geom::Geometry> _geometry;
    SpatialRelation _relation;
private:
    void collectResults(const geos::geom::Geometry *geomRelation, SPFeatureI inputfeature, std::vector<SPFeatureI>& outFeatures);
    static bool disjoint(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool contains(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool covers(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool coveredBy(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool touches(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool intersects(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool within(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool crosses(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool overlaps(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;
    static bool equals(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) ;

    NEW_OPERATION(SpatialRelationOperation);
};
}
}

#endif // SPATIALRELATION_H
