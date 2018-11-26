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

#ifndef GDALMODULE_H
#define GDALMODULE_H

#include <QObject>
#include <QLibrary>
#include "module.h"
#include "kernel.h"


namespace Ilwis {
namespace Gdal {

class GdalModule : public Module
{
public:
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit GdalModule(QObject *parent = 0);
    ~GdalModule();

    QString getInterfaceVersion() const;
    QString getName() const;
    QString getVersion() const;
    void getOperations(QVector<ICommandInfo *>& commands) const;
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.gdalconnector" FILE  "gdalconnector.json")

private:


signals:
};
}
}

#endif // GDALMODULE_H
