#ifndef CYLINDSINUSINTERRUPT2_H
#define CYLINDSINUSINTERRUPT2_H

namespace Ilwis {
namespace Internal {
class CylindSinusInterrupt2 : public ProjectionImplementationInternal
{
public:
    CylindSinusInterrupt2(const Ilwis::Resource &resource);
    ~CylindSinusInterrupt2();
    Coordinate pl2crd(const PhiLam&) const;
    PhiLam crd2pl(const Coordinate&) const;
    static bool canUse(const Ilwis::Resource &) ;
    bool prepare(const QString &parms = "");
    ProjectionImplementation* clone();
private:
    CylindSinusInterrupt2();
};
}
}

#endif // CYLINDSINUSINTERRUPT2_H
