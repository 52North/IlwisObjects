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

#ifndef POSTGRESQLTABLELOADER_H
#define POSTGRESQLTABLELOADER_H

namespace Ilwis {

class Resource;
class FeatureCoverage;
class RasterCoverage;

namespace Postgresql {


class PostgresqlTableLoader
{
public:
    PostgresqlTableLoader(const Resource &resource, const IOOptions &options);
    ~PostgresqlTableLoader();

    bool loadMetadata(Table *table) const;
    bool loadData(Table *table) const;

private:
    Resource _resource;
    IOOptions _options;

    QString select(QString columns) const;
    bool createColumnDefinition(Table *table, const QSqlQuery &query, QList<QString> &primaryKeys) const;
};

}
}
#endif // POSTGRESQLTABLELOADER_H
