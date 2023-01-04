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
#include "version.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorfactory.h"
#include "catalogconnectorfactory.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "netcdfobjectfactory.h"
#include "connectorinterface.h"
#include "ilwiscontext.h"
#include "netcdfmodule.h"
#include "dataformat.h"
#include "netcdfcatalogconnector.h"
#include "netcdfrasterconnector.h"

using namespace Ilwis;
using namespace NetCdf;

NetCdfModule::NetCdfModule(QObject *parent) : Module(parent, "NetCdfModule", "iv40","1.0")
{

}

NetCdfModule::~NetCdfModule()
{

}

QString NetCdfModule::getInterfaceVersion() const
{
      return "iv40";
}

QString NetCdfModule::getName() const
{
    return "netcdf plugin";
}

QString NetCdfModule::getVersion() const
{
    return "1.0";
}

void NetCdfModule::prepare()
{

    NetCdfObjectFactory *factory = new NetCdfObjectFactory();
    factory->prepare();
    kernel()->addFactory(factory );

    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return ;

    //inputconnectors
    cfactory->addCreator(itRASTER  ,"netcdf", NetCdfRasterConnector::create);

    //outputconnectors

    QFileInfo ilw = context()->ilwisFolder();
    QString path = ilw.canonicalFilePath() + "/extensions/netcdfconnector/resources";
    DataFormat::setFormatInfo(path + "/netcdfformats.config","netcdf");

    IlwisObject::addTypeFunction(NetCdfModule::ilwisType);

    kernel()->issues()->log("Loaded NetCDF module",IssueObject::itMessage);
}

IlwisTypes NetCdfModule::ilwisType(const QString &name) {
    if ( name.indexOf(".ilwis") > 0){
        Resource res = mastercatalog()->name2Resource(name);
        return res.ilwisType();
    }
    return itUNKNOWN;
}
