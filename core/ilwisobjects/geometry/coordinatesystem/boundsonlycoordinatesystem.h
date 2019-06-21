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

#ifndef BOUNDSONLYCOORDINATESYSTEM_H
#define BOUNDSONLYCOORDINATESYSTEM_H

namespace Ilwis{

class KERNELSHARED_EXPORT BoundsOnlyCoordinateSystem : public CoordinateSystem
{
public:
    BoundsOnlyCoordinateSystem();
    BoundsOnlyCoordinateSystem(const Ilwis::Resource &resource);

    virtual Coordinate coord2coord(const ICoordinateSystem& sourceCs, const Coordinate& crdSource) const;
    virtual LatLon coord2latlon(const Coordinate &crdSource) const;
    virtual Coordinate latlon2coord(const LatLon& ll) const;
    virtual bool isLatLon() const;
    virtual bool isUnknown() const;
    IlwisTypes ilwisType() const;
    QString toWKT(quint32 spaces=0) const;
	bool isCompatibleWith(const IlwisObject *obj, bool strict = false) const override;
};
typedef IlwisData<Ilwis::BoundsOnlyCoordinateSystem> IBoundsOnlyCoordinateSystem;
}

Q_DECLARE_METATYPE(Ilwis::IBoundsOnlyCoordinateSystem);

#endif // BOUNDSONLYCOORDINATESYSTEM_H
