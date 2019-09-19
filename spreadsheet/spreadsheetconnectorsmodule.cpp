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
#include "spreadsheetconnectorsobjectfactory.h"
#include "connectorinterface.h"
#include "ilwiscontext.h"
#include "spreadsheettableconnector.h"
#include "spreadsheetconnectorsmodule.h"
#include "dataformat.h"


using namespace Ilwis;
using namespace SpreadSheetConnectors;

SpreadSheetConnectorsModule::SpreadSheetConnectorsModule(QObject *parent) : Module(parent, "SpreadSheetConnectorsModule", "iv40","1.0")
{

}

SpreadSheetConnectorsModule::~SpreadSheetConnectorsModule()
{

}

QString SpreadSheetConnectorsModule::getInterfaceVersion() const
{
      return "iv40";
}

QString SpreadSheetConnectorsModule::getName() const
{
    return "Spread sheet plugin";
}

QString SpreadSheetConnectorsModule::getVersion() const
{
    return "1.0";
}

void SpreadSheetConnectorsModule::prepare()
{
    SpreadSheetConnectorsObjectFactory *factory = new SpreadSheetConnectorsObjectFactory();
    factory->prepare();
    kernel()->addFactory(factory );

    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return ;
    cfactory->addCreator(itTABLE,"spreadsheets",SpreadSheetTableConnector::create);

    cfactory->addCreator("spreadsheet","spreadsheets",SpreadSheetTableConnector::create);

    QFileInfo ilw = context()->ilwisFolder();
    QString path = ilw.canonicalFilePath() + "/extensions/spreadsheet/resources";
    DataFormat::setFormatInfo(path + "/spreadsheetformats.config","spreadsheets");

    QVariantList names = DataFormat::getFormatProperties(DataFormat::fpCODE,itTABLE,"spreadsheets");
    for(const QVariant& name : names)
        cfactory->addCreator(name.toString(),"spreadsheets", SpreadSheetTableConnector::create);

  kernel()->issues()->log("Loaded spreadsheet module",IssueObject::itMessage);

}
