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

#ifndef CATALOGEXPLORER_H
#define CATALOGEXPLORER_H

#include "kernel_global.h"
#include "kernel.h"
#include <QObject>

namespace Ilwis {
class KERNELSHARED_EXPORT CatalogExplorer : public QObject
{
    Q_OBJECT

public:
    enum ExplorerType{etIMPLICIT, etEXPLICIT};
    CatalogExplorer();
    CatalogExplorer(const Resource& resource,const IOOptions& options=IOOptions());
    virtual ~CatalogExplorer();

    virtual std::vector<Resource> loadItems(const IOOptions &options=IOOptions()) = 0;
    virtual bool canUse(const Resource& res) const = 0;
    virtual QString provider() const = 0;
    Resource source() const;
    virtual QFileInfo toLocalFile(const QUrl &datasource) const = 0;
    QFileInfo resolve2Local() const;
    virtual ExplorerType explorerType() const;

protected:
    IOOptions ioOptions() const;
    IOOptions& ioOptionsRef();

private:
    Resource _resource;
    IOOptions _options;

};
}

#define NEW_CATALOGEXPLORER(name) \
    private: \
static name *dummy_explorer;

#define REGISTER_CATALOGEXPLORER(name) \
    name *name::dummy_explorer = ConnectorFactory::registerCatalogExplorer(name::create);


#endif // CATALOGEXPLORER_H
