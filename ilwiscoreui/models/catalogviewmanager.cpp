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
#include "ilwisdata.h"
#include "catalog.h"
#include "catalogmodel.h"
#include "mastercatalog.h"
#include "catalogviewmanager.h"

using namespace Ilwis;
using namespace Ui;

Ilwis::Ui::CatalogViewManager* Ilwis::Ui::catalogViewManager(QObject *parent) {
    if (Ilwis::Ui::CatalogViewManager::_catalogViewManager == 0) {
        Ilwis::Ui::CatalogViewManager::_catalogViewManager = new Ilwis::Ui::CatalogViewManager(parent);
        Ilwis::Ui::CatalogViewManager::_catalogViewManager->connect(mastercatalog(), &MasterCatalog::contentChanged, Ilwis::Ui::CatalogViewManager::_catalogViewManager, &Ilwis::Ui::CatalogViewManager::updateCatalogViews) ;
    }
    return Ilwis::Ui::CatalogViewManager::_catalogViewManager;
}



CatalogViewManager *CatalogViewManager::_catalogViewManager = 0;
QTimer *CatalogViewManager::_timer = 0;



CatalogViewManager::CatalogViewManager(QObject *parent) : QObject(parent)
{

}

void CatalogViewManager::updateCatalogViews(const UrlSet &containers)
{
    Locker<std::mutex> lock(_lock);
    int typesNeeded = 0;
    for(auto url : containers){
        QString location = url.toString();
        if ( location.indexOf("ilwis://operations") == 0)
            typesNeeded |= CatalogModel::ctOPERATION;
        if ( location.indexOf(INTERNAL_CATALOG) == 0)
            typesNeeded |= CatalogModel::ctDATA;
        if ( url.scheme() == "file"){
            typesNeeded |= CatalogModel::ctDATA;
        }
    }
    for(auto& catalogViewItem : _catalogViews){
        int types = catalogViewItem.first->catalogType();
        int select = CatalogModel::ctBOOKMARK|CatalogModel::ctFIXED;
        if ( hasType(types,typesNeeded) && !hasType(types, select)){
            catalogViewItem.second = true;
        }
    }
}

void CatalogViewManager::doUpdate()
{
    Locker<std::mutex> lock(_lock);
    for(auto& catalogViewItem : _catalogViews){
        if ( catalogViewItem.second){
            catalogViewItem.first->refresh();
            catalogViewItem.second = false;
        }
    }
}

void CatalogViewManager::registerCatalogModel(CatalogModel *model)
{
    Locker<std::mutex> lock(_lock);
    auto iter = _catalogViews.find(model);
    if ( iter == _catalogViews.end()){
        if ( CatalogViewManager::_catalogViewManager->_timer == 0){
            CatalogViewManager::_catalogViewManager->_timer = new QTimer(CatalogViewManager::_catalogViewManager);
            CatalogViewManager::_catalogViewManager->_timer->connect(CatalogViewManager::_catalogViewManager->_timer, &QTimer::timeout, CatalogViewManager::_catalogViewManager, &CatalogViewManager::doUpdate);
            CatalogViewManager::_catalogViewManager->_timer->start(1500);
        }
        _catalogViews.insert(std::pair<CatalogModel *, bool>(model, false));
        model->connect(mastercatalog(), &MasterCatalog::contentChanged, model, &CatalogModel::contentChanged2);
    }
}

void CatalogViewManager::unRegisterCatalogModel(CatalogModel *model)
{
    Locker<std::mutex> lock(_lock);
    auto iter = _catalogViews.find(model);
    if ( iter != _catalogViews.end())
        _catalogViews.erase(iter);
}
