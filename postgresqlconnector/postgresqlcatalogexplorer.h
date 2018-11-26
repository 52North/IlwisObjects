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

#ifndef POSTGRESQLCATALOGEXPLORER_H
#define POSTGRESQLCATALOGEXPLORER_H

namespace Ilwis {
namespace Postgresql {


class PostgresqlCatalogExplorer : public CatalogExplorer
{
public:
    PostgresqlCatalogExplorer(const Resource &resource, const IOOptions &options=IOOptions());
    ~PostgresqlCatalogExplorer();

    static CatalogExplorer *create(const Resource &resource,const IOOptions &options=IOOptions());

    virtual std::vector<Resource> loadItems(const IOOptions &options=IOOptions());

    bool canUse(const Resource& resource) const;

    QString provider() const;

    /*
     * does not have any purpose in the WFS catalog connector
     */
    QFileInfo toLocalFile(const QUrl& ) const {
        QFileInfo fileInfo;
        return fileInfo;
    }

private:

    NEW_CATALOGEXPLORER(PostgresqlCatalogExplorer);

    IOOptions merge(const IOOptions &first, const IOOptions &second);
};
}
}

#endif // POSTGRESQLCATALOGEXPLORER_H
