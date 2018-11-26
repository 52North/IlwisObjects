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

#ifndef ILWIS3CATALOGCONNECTOR_H
#define ILWIS3CATALOGCONNECTOR_H

namespace Ilwis {

namespace Ilwis3 {

class Ilwis3CatalogConnector : public FileCatalogConnector
{
public:
    Ilwis3CatalogConnector(const Ilwis::Resource &resource);

    virtual bool loadItems();
    bool canUse(const Resource& resource) const;
    QString provider() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool);
private:
    void registerNames(const QString &name, QHash<QString, quint64> &names);
};
}
}

#endif // ILWIS3CATALOGCONNECTOR_H
