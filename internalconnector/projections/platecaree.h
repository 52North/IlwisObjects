#ifndef PLATECAREE_H
#define PLATECAREE_H

namespace Ilwis {
namespace Internal {
class PlateCaree : public ProjectionImplementationInternal
{
public:
    PlateCaree(const Ilwis::Resource &resource);
    ~PlateCaree();
    Coordinate pl2crd(const PhiLam&) const;
    PhiLam crd2pl(const Coordinate&) const;
    static bool canUse(const Ilwis::Resource &);
    bool prepare(const QString &parms = "");
    ProjectionImplementation *clone();
private:
    PlateCaree();
};
}
}

#endif // PLATECAREE_H
