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

#include <QString>
#include <functional>

#include "kernel.h"
#include "ilwis.h"
 //#include "angle.h"
//#include "point.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "geos/geom/Coordinate.h"
#include "ilwiscoordinate.h"
#include "location.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "projectionimplementation.h"
#include "box.h"
#include "coordinatesystem.h"
#include "proj4parameters.h"
#include "conventionalcoordinatesystem.h"
#include "projectionimplementationinternal.h"
#include "platecaree.h"

using namespace Ilwis;
using namespace Internal;

PlateCaree::PlateCaree(const Ilwis::Resource &resource) : ProjectionImplementationInternal(resource)
{

}

PlateCaree::~PlateCaree()
{
}

Coordinate PlateCaree::pl2crd(const PhiLam &pl) const
{
    return Coordinate(pl.Lam, pl.Phi);
}

PhiLam PlateCaree::crd2pl(const Coordinate &crd) const
{
    PhiLam pl;
    pl.Phi = crd.y;
    pl.Lam = crd.x;
    return pl;
}

bool PlateCaree::canUse(const Ilwis::Resource &resource)
{
    QString prj = resource.code();
    if ( prj == sUNDEF)
        return false;
    if ( prj == "PRJPC")
        return true;

    return false;
}

bool PlateCaree::prepare(const QString& parms){
    return true;
}

ProjectionImplementation *PlateCaree::clone()
{
    PlateCaree *csy = new PlateCaree();
    copyTo(csy);
    return csy;
}

PlateCaree::PlateCaree()
{

}



