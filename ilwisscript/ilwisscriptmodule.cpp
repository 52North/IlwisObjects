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

#include <QtPlugin>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "kernel.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "ilwisscriptmodule.h"
#include "ilwisdata.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "script.h"

using namespace Ilwis;

IlwisScriptModule::IlwisScriptModule(QObject *parent) :
    Module(parent, "IlwisScript", "iv40","1.0")
{
}

QString IlwisScriptModule::getInterfaceVersion() const
{
    return "iv40";

}

void IlwisScriptModule::prepare()
{
    commandhandler()->addOperation(Script::createMetadata(), Script::create);

    kernel()->issues()->log("Loaded module Internal Script module",IssueObject::itMessage);

}

QString IlwisScriptModule::name() const
{
    return "IlwisScript plugin";
}

QString IlwisScriptModule::version() const
{
    return "1.0";
}






