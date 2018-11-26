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

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QObject>
#include <unordered_map>
#include <map>
#include <set>
#include <memory>


namespace Ilwis {
class Resource;

namespace Ui{
class ResourceModel;
class MasterCatalogModel;

class ResourceManager : public QObject
{
    Q_OBJECT

public:
    friend std::unique_ptr<ResourceManager> &resourcemanager();

    ResourceManager();

    ResourceModel *createResourceModel(const QString& type, const Resource& resource);
    void deref(const QString &model, quint64 id, bool withDelete=true);

    static std::unique_ptr<ResourceManager> _manager;

private slots:
    //void containerChanged(const QString&);
private:


    std::map<QString, ResourceModel *> _resources;

    ResourceModel *findResource(const QString &model, const Ilwis::Resource& resource) const;
    void addResource(ResourceModel *rm);
    //MasterCatalogModel *_mastercatalogmodel = 0;
    QString key(const QString& type, quint64 id) const{ return QString::number(id) + type;}
};
}
}

std::unique_ptr<Ilwis::Ui::ResourceManager> &resourcemanager();

#endif // RESOURCEMANAGER_H
