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

#ifndef ILWIS3MODULE_H
#define ILWIS3MODULE_H

#include <QObject>
#include "module.h"
#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"

namespace Ilwis {
class InternalDatabaseConnectionQSqlQuery;

namespace Internal {

class InternalModule : public Module
{
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)

public:
    explicit InternalModule(QObject *parent = 0);

    QString getInterfaceVersion() const;
    QString name() const;
    QString version() const;
    void prepare();

private:
    bool createSpecialDomains();
    bool createPcs(InternalDatabaseConnection &db);
    bool createItems(InternalDatabaseConnection &db, const QString &sqltable, const QString &folder, IlwisTypes type, const QString internalname="");

    Q_PLUGIN_METADATA(IID "n52.ilwis.internalconnector" FILE  "internalconnector.json")

    static IlwisTypes ilwisType(const QString &name);
};
}
}

#endif // ILWIS3MODULE_H
