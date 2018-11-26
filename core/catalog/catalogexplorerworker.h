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

#ifndef CATALOGEXPLORERWORKER_H
#define CATALOGEXPLORERWORKER_H

#include <QObject>
#include <memory>

#include "kernel.h"
#include "ilwisdata.h"
#include "iooptions.h"
#include "catalog.h"
#include "mastercatalog.h"

namespace Ilwis {

class Catalog;
class CatalogExplorer;
class IOOptions;

typedef std::shared_ptr<CatalogExplorer> SPCatalogExplorer;

}
class CatalogExplorerWorker : public QObject, private Ilwis::CalcLatLon {
    Q_OBJECT

public:
    CatalogExplorerWorker(std::vector<std::pair<Ilwis::SPCatalogExplorer, Ilwis::IOOptions>> explorers) : _explorers(explorers){}

public slots:
    void process();


signals:
    void finished();
    void updateContainer();

private:
    std::vector<std::pair<Ilwis::SPCatalogExplorer, Ilwis::IOOptions>> _explorers;
};

#endif // CATALOGEXPLORERWORKER_H
