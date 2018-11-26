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

#ifndef GDALCATALOGCONNECTOR_H
#define GDALCATALOGCONNECTOR_H

namespace Ilwis{
namespace Gdal{
class GdalCatalogExplorer : public CatalogExplorer
{
public:
    GdalCatalogExplorer(const Ilwis::Resource &resource,const IOOptions& options=IOOptions());

    std::vector<Ilwis::Resource> loadItems(const IOOptions &options=IOOptions());
    bool canUse(const Resource& resource) const;
    QString provider() const;

   // std::vector<QUrl> sources(const Ilwis::Resource &resource, const QStringList& filter, int options=CatalogConnector::foFULLPATHS) const;
    QFileInfo toLocalFile(const QUrl &datasource) const;


    static Ilwis::CatalogExplorer *create(const Resource &resource,const IOOptions& options=IOOptions());
protected:
    IlwisTypes _type;

private:
    void getTypes(const std::multimap<QString, DataFormat> &formats, const QString &ext, IlwisTypes & tp, IlwisTypes & extendedType) const;

    NEW_CATALOGEXPLORER(GdalCatalogExplorer);
};
}
}

#endif // GDALCATALOGCONNECTOR_H
