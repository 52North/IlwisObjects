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
#include <QScopedPointer>
#include <functional>
#include "kernel.h"

#include "kernel.h"
#include "raster.h"
#include "datadefinition.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectfactory.h"
#include "catalogconnectorfactory.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "gslmodule.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "dataformat.h"

using namespace Ilwis;
using namespace GSL;

GSLModule::GSLModule(QObject *parent) :
    Module(parent, "GSLModule", "iv40","1.0")
{
}

GSLModule::~GSLModule()
{
}

QString GSLModule::getInterfaceVersion() const
{
    return "iv40";

}

void GSLModule::prepare()
{
    kernel()->issues()->log("Loaded module Gnu Scientific Library (GSL) connector",IssueObject::itMessage);
}

QString GSLModule::getName() const
{
    return "GSL plugin";
}

QString GSLModule::getVersion() const
{
    return "1.0";
}






