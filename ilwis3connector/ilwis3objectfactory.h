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

#ifndef ILWIS3OBJECTFACTORY_H
#define ILWIS3OBJECTFACTORY_H

namespace Ilwis {
class Ilwis3ObjectFactory : public IlwisObjectFactory
{
public:
    Ilwis3ObjectFactory();

    IlwisObject *create(const Ilwis::Resource &resource,const IOOptions& options=IOOptions()) const;
    bool canUse(const Resource &resource) const;
    bool prepare();
    std::vector<Resource> loadResource(const QUrl &url, IlwisTypes types=itILWISOBJECT) const;
private:
    void loadIlwis3Aliasses();
    void insertFile(const QString &filename, Ilwis::InternalDatabaseConnection &internalDb);
    bool fillEllipsoidRecord(const QStringList &parts, InternalDatabaseConnection &internalDb);
    bool fillDatumRecord(const QStringList &parts, InternalDatabaseConnection &internalDb);
    bool fillProjectionRecord(const QStringList &parts, InternalDatabaseConnection &internalDb);
    bool fillDomainRecord(const QStringList &parts, InternalDatabaseConnection &internalDb);
    bool doQuery(QString& query, InternalDatabaseConnection &internalDb);

    QHash<IlwisTypes,CheckUsage> _types;
};
}

#endif // ILWIS3OBJECTFACTORY_H
