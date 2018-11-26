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
