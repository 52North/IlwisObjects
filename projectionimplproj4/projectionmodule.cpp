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

#include "kernel.h"
#include "ilwis.h"
#include "geos/geom/Coordinate.h"
#include "ilwiscoordinate.h"
#include "location.h"
#include "projectionmodule.h"
#include "factory.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "projectionfactory.h"
#include "prjmplfactoryproj4.h"

using namespace Ilwis;

ProjectionModule::ProjectionModule(QObject *parent) :
    Module(parent, "projectionimplproj4", "iv40","1.0")
{
}

void Ilwis::ProjectionModule::prepare()
{
    FactoryInterface *factory = new ProjectionImplFactoryProj4();
    factory->prepare();
    kernel()->addFactory(factory );

    kernel()->issues()->log("Loaded Proj4 module",IssueObject::itMessage);
}




