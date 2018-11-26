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
class InternalCatalogConnector : public CatalogConnector
{
public:
    InternalCatalogConnector(const Ilwis::Resource &, bool load=true);
    ~InternalCatalogConnector();
    static ConnectorInterface *create(const Resource &, bool);


    bool prepare();
    std::vector<QUrl> sources(const QStringList& filter, int options=foFULLPATHS) const;
    QFileInfo toLocalFile(const QUrl &url) const;
    bool loadItems();

    bool isValid() const;
    bool canUse(const Resource &resource) const;
    QString provider() const;

private:
    bool createItems(QSqlQuery &db, const QString &table, IlwisTypes type);
    bool createPcs(QSqlQuery &db);
    bool createSpecialDomains();
};
}
}

#endif // INTERNALCATALOGCONNECTOR_H
