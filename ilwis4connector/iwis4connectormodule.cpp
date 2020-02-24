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
#include "ilwis4objectfactory.h"
#include "connectorfactory.h"
#include "iwis4connectormodule.h"
#include "table.h"
#include "feature.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "ilwisobjectconnector.h"
#include "ilwis4connector.h"
#include "ilwis4tableconnector.h"
#include "ilwis4domainconnector.h"
#include "ilwis4rasterconnector.h"
#include "ilwis4coordinatesystemconnector.h"
#include "dataformat.h"
#include "ilwiscontext.h"


using namespace Ilwis;
using namespace Ilwis4C;

Ilwis4ConnectorModule::Ilwis4ConnectorModule(QObject *parent) : 
    Module(parent, "Ilwis4ConnectorModule", "iv40","1.0")
{
}

QString Ilwis4ConnectorModule::getInterfaceVersion() const
{
    return "iv40";

}

void Ilwis4ConnectorModule::prepare()
{

	Ilwis4ObjectFactory *objfactory = new Ilwis4ObjectFactory(); 
	objfactory->prepare();
	kernel()->addFactory(objfactory);

	ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
	if (!factory)
		return;

	factory->addCreator(itTABLE, "ilwis4", Ilwis4TableConnector::create);
	factory->addCreator(itDOMAIN, "ilwis4", Ilwis4DomainConnector::create);
	factory->addCreator(itRASTER, "ilwis4", Ilwis4RasterConnector::create);
	factory->addCreator(itCOORDSYSTEM, "ilwis4", Ilwis4CoordinateSystemConnector::create);
	   

	factory->addCreator("i4table", "ilwis4", Ilwis4TableConnector::create);
	factory->addCreator("i4raster", "ilwis4", Ilwis4RasterConnector::create);
	factory->addCreator("i4domain", "ilwis4", Ilwis4DomainConnector::create);
	factory->addCreator("i4coordinatesystem", "ilwis4", Ilwis4CoordinateSystemConnector::create);


	QFileInfo ilw = context()->ilwisFolder();
	QString path = ilw.canonicalFilePath() + "/extensions/ilwis4connector/resources";
	DataFormat::setFormatInfo(path + "/ilwis4formats.config", "ilwis4");

	kernel()->issues()->log("Loaded Ilwis4 connector module", IssueObject::itMessage);
}


QString Ilwis4ConnectorModule::name() const
{
    return "Ilwis4 connector plugin";
}

QString Ilwis4ConnectorModule::version() const
{
    return "1.0";
}

IlwisTypes Ilwis4ConnectorModule::ilwisType(const QString &name) {
	if (name.indexOf(".ilwis4") > 0) {
		Resource res = mastercatalog()->name2Resource(name);
		return res.ilwisType();
	}
	return itUNKNOWN;
}






