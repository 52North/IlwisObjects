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
#include "hydroflow_module.h"


using namespace Ilwis;
using namespace Hydroflow;

Hydroflow_module::Hydroflow_module(QObject *parent) :
    Module(parent, "Hydroflow", "iv40","1.0")
{
}

QString Hydroflow_module::getInterfaceVersion() const
{
    return "iv40";
}

void Hydroflow_module::prepare()
{
  kernel()->issues()->log("Loaded Hydroflow module", IssueObject::itMessage);
}

QString Hydroflow_module::name() const
{
    return "Hydroflow plugin";
}

QString Hydroflow_module::version() const
{
    return "1.0";
}
