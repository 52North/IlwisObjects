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
#include "catalog.h"
#include "catalogexplorer.h"
#include "oshelper.h"
#include "catalogexplorerworker.h"

void CatalogExplorerWorker::process(){
    try{
        for(auto explorer : _explorers){
            std::vector<Ilwis::Resource> items = explorer.first->loadItems(explorer.second);
            Ilwis::mastercatalog()->addItems(items, true);
        }
        Ilwis::Resource location;
        if ( _explorers.size() > 0){
            location = _explorers[0].first->source();
        }

        if ( location.isValid() && Ilwis::Catalog::isSpatialDataLocation(location.url())){
            QString query = QString("(type & %1) != 0 and container='%2'").arg(QString::number(itCOVERAGE)).
                    arg(Ilwis::OSHelper::neutralizeFileName(location.url().toString()));
            calculatelatLonEnvelopes(query, location.name());

        }
        emit finished();
       return;

    } catch(const Ilwis::ErrorObject& ){

    } catch ( const std::exception& ex){
        Ilwis::kernel()->issues()->log(ex.what());
    }
    emit finished();
}
