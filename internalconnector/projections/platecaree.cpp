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



