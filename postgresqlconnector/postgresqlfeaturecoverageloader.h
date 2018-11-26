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

#ifndef POSTGRESQLFEATURECOVERAGELOADER_H
#define POSTGRESQLFEATURECOVERAGELOADER_H

namespace Ilwis {

namespace Postgresql {

struct MetaGeometryColumn;

class PostgresqlFeatureCoverageLoader
{
public:
    PostgresqlFeatureCoverageLoader(const Resource &resource, const IOOptions &options);
    ~PostgresqlFeatureCoverageLoader();

    bool loadMetadata(FeatureCoverage *fcoverage) const;
    bool loadData(FeatureCoverage *fcoverage) const;
    bool storeData(FeatureCoverage *fcoverage) const;

private:
    Resource _resource;
    IOOptions _options;

    void setFeatureCount(FeatureCoverage *fcoverage) const;
    void setSpatialMetadata(FeatureCoverage *fcoverage) const;
    void setSubfeatureSemantics(Ilwis::FeatureCoverage *fcoverage, Ilwis::IDomain &semantics) const;

    QString selectGeometries(const QList<MetaGeometryColumn> &metaGeometry) const;
    geos::geom::Geometry* createGeometry(QSqlQuery &query, QString geomColumn, ICoordinateSystem crs) const;
};


}
}

#endif // POSTGRESQLFEATURECOVERAGELOADER_H
