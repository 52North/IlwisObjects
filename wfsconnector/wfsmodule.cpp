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
#include "ilwisdata.h"
#include "geometries.h"
#include "geos/geom/Geometry.h"
#include "range.h"
#include "domain.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "coordinatesystem.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "attributedefinition.h"
#include "table.h"
#include "catalog.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogconnectorfactory.h"
#include "ilwisobjectconnector.h"
#include "symboltable.h"
#include "wfsmodule.h"
#include "wfsparsingcontext.h"
#include "wfsfeatureconnector.h"
#include "catalogconnector.h"
#include "catalogexplorer.h"
#include "wfscatalogexplorer.h"
#include "ilwisobjectfactory.h"
#include "wfsobjectfactory.h"
#include "wfsresponse.h"
#include "wfs.h"

using namespace Ilwis;
using namespace Wfs;

WfsModule::WfsModule(QObject *parent) :
    Module(parent, "WfsModule", "iv40","1.0")
{
}

WfsModule::~WfsModule()
{
}

QString WfsModule::getInterfaceVersion() const
{
    return "iv40";
}

void WfsModule::prepare()
{
    WfsObjectFactory *factory = new WfsObjectFactory();
    factory->prepare();
    kernel()->addFactory(factory);

    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return;


    cfactory->addCreator(itCATALOG, "wfs", CatalogConnector::create);
    cfactory->addCreator(itFEATURE, "wfs", WfsFeatureConnector::create);

    kernel()->issues()->log("Loaded wfs module",IssueObject::itMessage);
}

QString WfsModule::getName() const
{
    return "WFS plugin";
}

QString WfsModule::getVersion() const
{
    return "1.0";
}

