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
#include "proj4parameters.h"
#include "cylindsinusinterrupt2.h"

using namespace Ilwis;
using namespace Internal;

const double PI_60 = (M_PI/60);

Ilwis::Internal::CylindSinusInterrupt2::CylindSinusInterrupt2(const Ilwis::Resource &resource) : ProjectionImplementationInternal(resource)
{

}

CylindSinusInterrupt2::~CylindSinusInterrupt2()
{

}

Coordinate CylindSinusInterrupt2::pl2crd(const PhiLam &pl) const
{
    Coordinate xy;
    if (abs(pl.Phi)> M_PI_2) return xy;
    if (abs(pl.Lam)> M_PI) return xy;
    if (pl.Lam < - 6 * PI_60)
        xy.x = (pl.Lam + 33 * PI_60) * cos(pl.Phi) - 33 * PI_60;
    else if (pl.Lam < 20 * PI_60)
        xy.x = (pl.Lam - 7 * PI_60) * cos(pl.Phi) + 7 * PI_60;
    else
        xy.x = (pl.Lam - 40 * PI_60) * cos(pl.Phi) + 40 * PI_60;

    xy.y = pl.Phi;
    return xy;
}

PhiLam CylindSinusInterrupt2::crd2pl(const Coordinate &xy) const
{
    PhiLam pl;
    double cosfi = cos(xy.y);
    if (cosfi < EPS10) return pl;  // latitude beyond poles

    if (abs(xy.x + 33 * PI_60)/cosfi <= 27 * PI_60)   // Sector from -180 to -10
        pl.Lam = (xy.x + 33 * PI_60) / cosfi - 33 * PI_60;
    else if (abs(xy.x - 7 * PI_60)/cosfi <= 13 * PI_60)  // Sector from -10 to 60
        pl.Lam = (xy.x - 7 * PI_60) / cosfi + 7 * PI_60;
    else if (abs(xy.x - 40 * PI_60)/cosfi <= 20 * PI_60)  // Sector from 60 to 180
        pl.Lam = (xy.x - 40 * PI_60) / cosfi + 40 * PI_60;
    else return pl;

    pl.Phi = xy.y;
    return pl;
}

bool CylindSinusInterrupt2::canUse(const Ilwis::Resource &resource)
{
    QString prj = resource.code();
    if ( prj == sUNDEF)
        return false;
    if ( prj == "PRJSINI2")
        return true;

    return false;
}

bool CylindSinusInterrupt2::prepare(const QString &parms)
{
    if ( parms != "") {
        Proj4Parameters proj4(parms);
    }
    return true;
}

ProjectionImplementation *CylindSinusInterrupt2::clone()
{
    CylindSinusInterrupt2 *csy = new CylindSinusInterrupt2();
    copyTo(csy);
    return csy;
}

CylindSinusInterrupt2::CylindSinusInterrupt2()
{

}


