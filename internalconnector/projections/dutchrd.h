#ifndef DUTCHRD_H
#define DUTCHRD_H

namespace Ilwis {
namespace Internal {
class DutchRD : public ProjectionImplementationInternal
{
public:
    DutchRD(const Ilwis::Resource &resource);
    ~DutchRD();
    Coordinate pl2crd(const PhiLam&) const;
    PhiLam crd2pl(const Coordinate&) const;
    static bool canUse(const Ilwis::Resource &) ;
    bool prepare(const QString &parms = "");
    virtual void setCoordinateSystem(ConventionalCoordinateSystem *csy);
    ProjectionImplementation* clone();
private:
    DutchRD();
    double _k0;
    double _rBOLSTRAAL;
    double _rNgauss;
    double _rMgauss;
    double _B0;
    double _ell_a;
    double _ell_e;
    double _ell_e2;
};
}
}

#endif // DUTCHRD_H
