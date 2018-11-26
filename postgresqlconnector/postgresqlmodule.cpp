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
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorfactory.h"
#include "catalogconnectorfactory.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "connectorinterface.h"
#include "catalogconnector.h"
#include "ilwiscontext.h"
#include "domain.h"
#include "dataformat.h"
#include "postgresqlobjectfactory.h"
#include "postgresqlconnector.h"
#include "postgresqlcatalogexplorer.h"
#include "postgresqlfeatureconnector.h"
#include "postgresqlrasterconnector.h"
#include "postgresqltableconnector.h"
#include "postgresqlmodule.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlModule::PostgresqlModule(QObject *parent) :
    Module(parent, "PostgresqlModule", "iv40","1.0")
{
}

PostgresqlModule::~PostgresqlModule()
{
}

QString PostgresqlModule::getInterfaceVersion() const
{
      return "iv40";
}

QString PostgresqlModule::getName() const
{
    return "PostgreSQL plugin";
}

QString PostgresqlModule::getVersion() const
{
    return "1.0";
}

void PostgresqlModule::prepare()
{
    PostgresqlObjectFactory *factory = new PostgresqlObjectFactory();
    factory->prepare();
    kernel()->addFactory(factory);
    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return ;
    cfactory->addCreator("table", "postgresql", PostgresqlTableConnector::create);
    cfactory->addCreator("simplefeatures", "postgresql", PostgresqlFeatureConnector::create);
    cfactory->addCreator("map", "postgresql", PostgresqlRasterConnector::create);
    cfactory->addCreator(itTABLE, "postgresql", PostgresqlTableConnector::create);
    cfactory->addCreator(itFEATURE, "postgresql", PostgresqlFeatureConnector::create);
    cfactory->addCreator(itRASTER, "postgresql", PostgresqlRasterConnector::create);
    cfactory->addCreator(itCATALOG, "postgresql", CatalogConnector::create);

    QFileInfo ilw = context()->ilwisFolder();
    QString path = ilw.canonicalFilePath() + "/extensions/postgresqlconnector/resources";
    DataFormat::setFormatInfo(path + "/postgresformats.config","postgresql");

    kernel()->issues()->log("Loaded Postgresql Module", IssueObject::itMessage);
}
