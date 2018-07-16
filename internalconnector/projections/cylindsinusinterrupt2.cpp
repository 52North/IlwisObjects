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

Coordinate CylindSinusInterrupt2::ll2crd(const LatLon &pl) const
{
    Coordinate xy;
    if (abs(pl.lat().radians())> M_PI/2.0) return xy;
    if (abs(pl.lon().radians())> M_PI) return xy;
    if (pl.lon().radians() < - 6 * PI_60)
     xy.x = (pl.lon().radians() + 33 * PI_60) * cos(pl.lat().radians()) - 33 * PI_60;
    else if (pl.lon().radians() < 20 * PI_60)
     xy.x = (pl.lon().radians() - 7 * PI_60) * cos(pl.lat().radians()) + 7 * PI_60;
    else
     xy.x = (pl.lon().radians() - 40 * PI_60) * cos(pl.lat().radians()) + 40 * PI_60;

    xy.y = pl.lat().radians();
    return xy;
}

LatLon CylindSinusInterrupt2::crd2ll(const Coordinate &xy) const
{
    LatLon pl;
     double cosfi = cos(xy.y);
     if (cosfi < EPS10) return pl;  // latitude beyond poles

     if (abs(xy.x + 33 * PI_60)/cosfi <= 27 * PI_60)   // Sector from -180 to -10
         pl.lon(Angle((xy.x + 33 * PI_60) / cosfi - 33 * PI_60, true));
     else if (abs(xy.x - 7 * PI_60)/cosfi <= 13 * PI_60)  // Sector from -10 to 60
         pl.lon(Angle((xy.x - 7 * PI_60) / cosfi + 7 * PI_60, true));
     else if (abs(xy.x - 40 * PI_60)/cosfi <= 20 * PI_60)  // Sector from 60 to 180
         pl.lon(Angle((xy.x - 40 * PI_60) / cosfi + 40 * PI_60, true));
     else return pl;

     pl.lat(Angle(xy.y, true));
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


