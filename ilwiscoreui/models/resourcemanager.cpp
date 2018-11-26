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

#include "kernel.h"
#include "uicontextmodel.h"
#include "mastercatalogmodel.h"
#include "catalogmodel.h"
#include "resourcemanager.h"

using namespace Ilwis;
using namespace Ui;

std::unique_ptr<ResourceManager> ResourceManager::_manager;

ResourceManager::ResourceManager()
{
   //connect(mastercatalog(), &MasterCatalog::contentChanged, this, &ResourceManager::containerChanged);
}

ResourceModel *ResourceManager::createResourceModel(const QString &type, const Resource &resource)
{
    auto *model = findResource(type, resource);
    if ( !model){
        if ( type == "resourcemodel"){
            model = new ResourceModel(resource);
        }
        if ( type == "operationmodel"){
            model = new OperationModel(resource);
        }
        addResource(model );
    }else { // we found it but we update it the internal resource of the model to the info in the database
        Resource res = mastercatalog()->id2Resource(resource.id());
        model->resourceRef() = res;
    }
    model->addref();
    return model;
}

void ResourceManager::deref(const QString& type, quint64 id, bool withDelete)
{
    auto iter = _resources.find(key(type,id));
    if ( iter != _resources.end()){
        (*iter).second->deref();
        if ((*iter).second->ref() == 0){
            auto *model = (*iter).second;
            _resources.erase(iter);
            if ( withDelete){
                if ( model->modelType() == "resourcemodel") {// catalogtype of models are deleted by the system
                   // model->deleteLater();
                }
            }
        }
    }
}

//void ResourceManager::containerChanged(const QString &s)
//{
//    qDebug() << "detected 1" << s;
//}



std::unique_ptr<ResourceManager>& resourcemanager()
{
    if ( !ResourceManager::_manager) {
        ResourceManager::_manager.reset(new ResourceManager());
    }
    return ResourceManager::_manager;
}

ResourceModel *ResourceManager::findResource(const QString& model, const Resource &resource) const
{
    auto iter = _resources.find(key(model, resource.id()));
    if ( iter != _resources.end())
        return (*iter).second;
    return 0;

}

void ResourceManager::addResource(ResourceModel *rm)
{
    if ( !rm || !rm->item().isValid())
        return;

    ResourceModel *model = findResource(rm->modelType(),rm->item()) ;
    if ( model)
        return;
    _resources[key(rm->modelType(), rm->item().id())] = rm;


}

