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
class GdalCatalogConnector : public CatalogConnector
{
public:
    GdalCatalogConnector(const Ilwis::Resource &resource, bool load=true);
    bool prepare();

    virtual bool loadItems();
    bool canUse(const Resource& resource) const;
    QString provider() const;

    std::vector<QUrl> sources(const QStringList& filter, int options=foFULLPATHS) const;
    QFileInfo toLocalFile(const QUrl &datasource) const;


    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true);
protected:
    IlwisTypes _type;

private:
    IlwisTypes extendedType(const std::multimap<QString, DataFormat> &formats, const QString &ext) const;
};
}
}

#endif // GDALCATALOGCONNECTOR_H
