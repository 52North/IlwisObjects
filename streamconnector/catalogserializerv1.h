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

#ifndef CATALOGSERIALIZER_H
#define CATALOGSERIALIZER_H

namespace Ilwis {
namespace Stream {
class CatalogserializerV1 : public VersionedSerializer
{
public:
    CatalogserializerV1(QDataStream &stream, const QString &version) ;

    bool store(IlwisObject *obj, const Ilwis::IOOptions &options= IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    bool loadData(IlwisObject*, const IOOptions& options = IOOptions()) { return true; }
    bool loadItems(std::vector<Resource> &items);
    static VersionedSerializer *create(QDataStream &stream, const QString &version);
private:
    void adaptProperyResource(const QString& baseUrl, Resource& resource, const QString &propertyName) const;
    void adaptResource(const QString &baseUrl, Resource &resource) const;
    QString adaptedUrl(const QString &baseUrl, const Resource &resource, bool asRaw) const;
};
}
}

#endif // CATALOGSERIALIZER_H
