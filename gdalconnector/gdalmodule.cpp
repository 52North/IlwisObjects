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
#include "geometries.h"
#include "range.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "table.h"
#include "catalog.h"
#include "pixeliterator.h"
#include "gdalproxy.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "dataformat.h"
#include "gdalconnector.h"
#include "coordinatesystemconnector.h"
#include "coverageconnector.h"
#include "itemrange.h"
#include "colorrange.h"
#include "gridcoverageconnector.h"
#include "featurecoverage.h"
#include "feature.h"
#include "gdalfeatureconnector.h"
#include "gdalfeaturetableconnector.h"
#include "domainconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "domainconnector.h"
#include "georefconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogconnectorfactory.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "gdalmodule.h"
#include "catalogexplorer.h"
#include "dataformat.h"
#include "ilwisobjectfactory.h"
#include "gdalobjectfactory.h"
#include "gdalproxy.h"
#include "dataformat.h"

using namespace Ilwis;
using namespace Gdal;

GdalModule::GdalModule(QObject *parent) :
    Module(parent, "gdalconnector", "iv40","1.0")
{
}

GdalModule::~GdalModule()
{
    if ( gdal())
        delete gdal();
}

QString GdalModule::getInterfaceVersion() const
{
    return "iv40";

}

void GdalModule::prepare()
{
    GdalObjectFactory *factory = new GdalObjectFactory();
    factory->prepare();
    kernel()->addFactory(factory );

    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return ;


    cfactory->addCreator(itITEMDOMAIN | itNUMERICDOMAIN ,"gdal", DomainConnector::create);
    cfactory->addCreator(itRASTER ,"gdal", RasterCoverageConnector::create);
    cfactory->addCreator(itGEOREF,"gdal", GeorefConnector::create);
    cfactory->addCreator(itCOORDSYSTEM,"gdal", CoordinateSystemConnector::create);
    cfactory->addCreator(itFEATURE,"gdal", GdalFeatureConnector::create);
    cfactory->addCreator(itTABLE,"gdal", GdalFeatureTableConnector::create);
    cfactory->addCreator(itCATALOG,"gdal", CatalogConnector::create);

    int driverCount = gdal()->getDriverCount(); // calls GdalProxy::prepare() to initialize gdal()
    // add all vector creators
    QVariantList names = DataFormat::getFormatProperties(DataFormat::fpCODE,itFEATURE,"gdal");
    for(const QVariant& name : names)
        cfactory->addCreator(name.toString(),"gdal", GdalFeatureConnector::create);
    // then add all raster creators (all formats are looped, the ones that aren't added in the previous loop are raster formats)
    for (int i = 0; i < driverCount; ++i) {
        GDALDriverH driv = gdal()->getDriver(i);
        if (driv) {
            QString shortName = gdal()->getShortName(driv);
            cfactory->addCreator(shortName, "gdal", RasterCoverageConnector::create);
        }
    }

    IlwisObject::addTypeFunction(GdalConnector::ilwisType);

    kernel()->issues()->log("Loaded module GdalConnector",IssueObject::itMessage);
}

QString GdalModule::getName() const
{
    return "Gdal plugin";
}

QString GdalModule::getVersion() const
{
    return "1.0";
}

void GdalModule::getOperations(QVector<ICommandInfo *> &) const{
}




