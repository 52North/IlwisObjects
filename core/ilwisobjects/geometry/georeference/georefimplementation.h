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

#ifndef GEOREFIMPLEMENTATION_H
#define GEOREFIMPLEMENTATION_H

namespace Ilwis {
class KERNELSHARED_EXPORT GeoRefImplementation : public GeoRefInterface, public Identity
{
public:
    GeoRefImplementation(const QString& type);
    virtual ~GeoRefImplementation();
    ICoordinateSystem coordinateSystem() const;
    void coordinateSystem(const ICoordinateSystem& csy);
    Size<> size() const;
    void size(const Size<>& sz);
    virtual Envelope envelope() const;
    bool centerOfPixel() const;
    void centerOfPixel(bool yesno);
    int compute();
    virtual bool isLinear() const;
    virtual GeoRefImplementation *clone() = 0;
	virtual QString grfType() const = 0;
	virtual bool isValid() const = 0;
    virtual QString toCode() const;

    virtual bool isCompatible(const IGeoReference &georefOther) const;
protected:
    virtual void copyTo(GeoRefImplementation *impl);

    ICoordinateSystem _csy;
    Size<> _size;
    bool _centerOfPixel;
};
}

#endif // GEOREFIMPLEMENTATION_H
