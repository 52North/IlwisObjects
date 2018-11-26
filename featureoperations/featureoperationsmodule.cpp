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
#include "coverage.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "featureoperationsmodule.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "gridding.h"


using namespace Ilwis;
using namespace FeatureOperations;

FeatureOperationsModule::FeatureOperationsModule(QObject *parent) :
    Module(parent, "FeatureOperations", "iv40","1.0")
{
}

QString FeatureOperationsModule::getInterfaceVersion() const
{
    return "iv40";

}

void FeatureOperationsModule::prepare()
{
   commandhandler()->addOperation(Gridding::createMetadata(), Gridding::create);

   kernel()->issues()->log("Loaded module FeatureOperations",IssueObject::itMessage);

}

QString FeatureOperationsModule::name() const
{
    return "Feature Operations plugin";
}

QString FeatureOperationsModule::version() const
{
    return "1.0";
}






