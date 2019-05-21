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

#ifndef SIMPELGEOREFERENCE_H
#define SIMPELGEOREFERENCE_H

#include "kernel_global.h"

namespace Ilwis {
class GeoRefImplementation;

class KERNELSHARED_EXPORT SimpelGeoReference : public GeoRefImplementation
{
public:
    SimpelGeoReference();
    SimpelGeoReference(const QString &type);
    static GeoRefImplementation * create();
    GeoRefImplementation *clone();
    virtual Coordinate pixel2Coord(const Pixeld&) const;
    virtual Pixeld coord2Pixel(const Coordinate& crd) const;
    virtual double pixelSize() const;
    bool isCompatible(const IGeoReference &georefOther) const;
    virtual bool isLinear() const;

    std::vector<double> matrix() const;
    std::vector<double> support() const;
    static QString typeName();
	QString grfType() const;
	bool isValid() const;
protected:
    void copyTo(GeoRefImplementation *impl);
    void clear();
    double _a11,_a12,_a22,_a21,_b1,_b2;
    double _det;

};


}

#endif // SIMPELGEOREFERENCE_H
