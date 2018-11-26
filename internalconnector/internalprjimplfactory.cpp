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

#include <QUrl>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QStringList>

#include "kernel.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "projection.h"
#include "ilwisobjectfactory.h"
#include "projectionimplementation.h"
#include "projectionfactory.h"
#include "internalprjmplfactory.h"
#include "projectionimplementation.h"
#include "projections/projectionimplementationinternal.h"
#include "projections/platecaree.h"
#include "projections/cylindsinusinterrupt2.h"
#include "projections/dutchrd.h"

using namespace Ilwis;
using namespace Internal;

ProjectionImplFactory::ProjectionImplFactory() : ProjectionFactory("ProjectionFactory","internal")
{
}

ProjectionImplementation *ProjectionImplFactory::create(const Ilwis::Resource &resource) const{
    QString prj = resource.code();

    if ( prj == "PRJPC")
        return new PlateCaree(resource);
    if ( prj == "PRJSINI2")
        return new CylindSinusInterrupt2(resource);
    if ( prj == "PRJDRD")
        return new DutchRD(resource);

    return 0;
}

ProjectionImplementation *ProjectionImplFactory::create(const QString&,const QString &) const
{
    return nullptr;
}

bool ProjectionImplFactory::canUse(const Ilwis::Resource &resource) const
{
    QString prj = resource.code();
    if ( prj == "PRJPC")
        return true;
    if ( prj == "PRJSINI2")
        return true;
    if ( prj == "PRJDRD")
        return true;

    return false;
}

bool ProjectionImplFactory::prepare()
{
    return true;
}
