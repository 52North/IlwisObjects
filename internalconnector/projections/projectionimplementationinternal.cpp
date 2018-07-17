#include <QString>
#include <functional>
#include <QtMath>

#include "kernel.h"
#include "ilwis.h"
#include "geos/geom/Coordinate.h"
#ifdef Q_OS_WIN
#include "geos/geom/Coordinate.h"
#endif
#include "ilwiscoordinate.h"
#include "location.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "box.h"
#include "coordinatesystem.h"
#include "proj4parameters.h"
#include "conventionalcoordinatesystem.h"
#include "projectionimplementation.h"
#include "projectionimplementationinternal.h"

using namespace Ilwis;
using namespace Internal;

ProjectionImplementationInternal::ProjectionImplementationInternal(const Resource& resource) :
    ProjectionImplementation(resource.code()),
    _easting(0),
    _northing(0),
    _maxis(6371007.180918499800),
    _phi0(0),
    _lam0(0)
{
}

Coordinate ProjectionImplementationInternal::latlon2coord(const LatLon &ll) const
{
    if (_coordinateSystem->projection().isValid() && ll.isValid()) {
        PhiLam pl;
        pl.Phi = ll.Phi();
        pl.Lam = ll.Lambda();
        if (pl.Phi > M_PI_2)
            pl.Phi = M_PI_2;
        else if (pl.Phi < -M_PI_2)
            pl.Phi = -M_PI_2;
        pl.Lam -= _lam0;
        Coordinate xy = pl2crd(pl);
        if (xy == crdUNDEF)
            return crdUNDEF;
        Coordinate crd;
        crd.x = xy.x * _maxis  + _easting;
        crd.y = xy.y * _maxis  + _northing;
        return crd;
    }
    else
        return crdUNDEF;

}

LatLon ProjectionImplementationInternal::coord2latlon(const Ilwis::Coordinate &crdSource) const
{
    if (_coordinateSystem->projection().isValid() && crdSource != crdUNDEF) {
        Coordinate xy((crdSource.x - _easting) / _maxis, (crdSource.y - _northing) / _maxis);
        PhiLam pl = crd2pl(xy);
        if (pl.fUndef())
            return llUNDEF;
        if (abs(pl.Phi) > M_PI_2)
            return llUNDEF;
        pl.Lam += _lam0;
        pl.AdjustLon();
        LatLon ll;
        ll.Phi(pl.Phi);
        ll.Lambda(pl.Lam);
        return ll;
    }
    else
        return llUNDEF;

}

void ProjectionImplementationInternal::setCoordinateSystem(ConventionalCoordinateSystem *csy)
{
    ProjectionImplementation::setCoordinateSystem(csy);
    IProjection prj = _coordinateSystem->projection();
    if ( prj.isValid()) {
        _easting = prj->parameter(Projection::pvFALSEEASTING).toDouble();
        _northing = prj->parameter(Projection::pvFALSENORTHING).toDouble();
        _maxis = _coordinateSystem->ellipsoid()->majorAxis();
        _phi0 = prj->parameter(Projection::pvCENTRALPARALLEL).toDouble() * M_PI / 180;
        _lam0 = prj->parameter(Projection::pvCENTRALMERIDIAN).toDouble() * M_PI / 180;
    }
}

QString ProjectionImplementationInternal::toProj4() const
{
    return sUNDEF; // TODO!
}

bool ProjectionImplementationInternal::canConvertToLatLon() const
{
    return true;
}

bool ProjectionImplementationInternal::canConvertToCoordinate() const
{
    return true;
}

void ProjectionImplementationInternal::copyTo(ProjectionImplementation *prj)
{
    ProjectionImplementation::copyTo(prj);
    ProjectionImplementationInternal *impl = static_cast<ProjectionImplementationInternal *>(prj);
    impl->_easting = _easting;
    impl->_northing = _northing;
    impl->_maxis = _maxis;
    impl->_phi0 = _phi0;
    impl->_lam0 = _lam0;
}

ProjectionImplementationInternal::ProjectionImplementationInternal()
{

}


