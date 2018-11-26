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
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "ilwisobjectfactory.h"
#include "projectionimplementation.h"
#include "projectionfactory.h"
#include "prjmplfactoryproj4.h"
#include "proj_api.h"
#include "prjimplproj4.h"
#include "proj4parameters.h"

using namespace Ilwis;

ProjectionImplFactoryProj4::ProjectionImplFactoryProj4() : ProjectionFactory("ProjectionFactory","proj4")
{
    InternalDatabaseConnection db;
    QString query = QString("Select * from projection");
    if ( db.exec(query)) {
        while ( db.next()) {
             QSqlRecord rec = db.record();
             QString code = rec.field("code").value().toString();
             if ( code.left(3) != "PRJ")
                _knownProjections += "@" + code + "@";

        }
    }
}

ProjectionImplementation *ProjectionImplFactoryProj4::create(const Ilwis::Resource &resource) const{

    return new ProjectionImplementationProj4(resource);
}

ProjectionImplementation *ProjectionImplFactoryProj4::create(const QString& projtype,const QString &proj4def) const
{
    return new ProjectionImplementationProj4(projtype, proj4def);
}

bool ProjectionImplFactoryProj4::canUse(const Ilwis::Resource &resource) const
{
    QString prj = resource.code();
    if ( prj == sUNDEF)
        return false;
    if ( _knownProjections.indexOf("@" + prj + "@") != -1)
        return true;
    return false;
}

bool ProjectionImplFactoryProj4::prepare()
{

    return true;
}
