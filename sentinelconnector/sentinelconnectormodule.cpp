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
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "sentinelobjectfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectconnector.h"
#include "sentinelconnectormodule.h"
#include "sentinelrasterconnector.h"
#include "dataformat.h"
#include "ilwiscontext.h"


using namespace Ilwis;
using namespace Sentinel;

SentinelConnectorModule::SentinelConnectorModule(QObject *parent) : 
    Module(parent, "SentinelConnectorModule", "iv40","1.0")
{
}

QString SentinelConnectorModule::getInterfaceVersion() const
{
    return "iv40";

}

void SentinelConnectorModule::prepare()
{

	SentinelObjectFactory *objfactory = new SentinelObjectFactory(); 
	objfactory->prepare();
	kernel()->addFactory(objfactory);

	ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
	if (!factory)
		return;

	factory->addCreator(itRASTER, "Sentinel", SentinelRasterConnector::create);
	   

	factory->addCreator("i4raster", "Sentinel", SentinelRasterConnector::create);



	QFileInfo ilw = context()->ilwisFolder();
	QString path = ilw.canonicalFilePath() + "/extensions/sentinelconnector/resources";
	DataFormat::setFormatInfo(path + "/Sentinelformats.config", "Sentinel");

	kernel()->issues()->log("Loaded sentinel connector module", IssueObject::itMessage);
}


QString SentinelConnectorModule::name() const
{
    return "Sentinel connector plugin";
}

QString SentinelConnectorModule::version() const
{
    return "1.0";
}

IlwisTypes SentinelConnectorModule::ilwisType(const QString &name) {
	return itUNKNOWN;
}






