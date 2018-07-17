#ifndef PROJECTIONIMPLEMENTATIONINTERNAL_H
#define PROJECTIONIMPLEMENTATIONINTERNAL_H

namespace Ilwis {
namespace Internal {
class ProjectionImplementationInternal : public ProjectionImplementation
{
public:
    ProjectionImplementationInternal(const Resource& resource);

    Coordinate latlon2coord(const LatLon&) const;
    LatLon coord2latlon(const Coordinate&) const;
    virtual void setCoordinateSystem(ConventionalCoordinateSystem *csy);
    QString toProj4() const;
    bool canConvertToLatLon() const;
    bool canConvertToCoordinate() const;
protected:
    void copyTo(ProjectionImplementation *prj);
    ProjectionImplementationInternal();
    virtual Coordinate pl2crd(const PhiLam&) const = 0;
    virtual PhiLam crd2pl(const Coordinate&) const = 0;

    double _easting;
    double _northing;
    double _maxis;
    //double _centralMeridian;
    double _phi0;
    double _lam0;

};
}
}

#endif // PROJECTIONIMPLEMENTATIONINTERNAL_H
