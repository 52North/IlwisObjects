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

#ifndef INTERNALCATALOGCONNECTOR_H
#define INTERNALCATALOGCONNECTOR_H

namespace Ilwis {
namespace Internal {
class InternalCatalogExplorer : public CatalogExplorer
{
public:
    InternalCatalogExplorer(const Ilwis::Resource &,const IOOptions& options=IOOptions());
    ~InternalCatalogExplorer();
    static CatalogExplorer *create(const Resource &,const IOOptions& options=IOOptions());

    QFileInfo toLocalFile(const QUrl &url) const;
    std::vector<Resource> loadItems(const IOOptions& options=IOOptions());

    bool isValid() const;
    bool canUse(const Resource &resource) const;
    QString provider() const;

    NEW_CATALOGEXPLORER(InternalCatalogExplorer);
};
}
}

#endif // INTERNALCATALOGCONNECTOR_H
