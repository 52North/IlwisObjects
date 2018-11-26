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

#ifndef GEOREFADAPTER_H
#define GEOREFADAPTER_H

namespace Ilwis {
class GeoRefAdapter
{
public:
    GeoRefAdapter();
    virtual Pixeld adaptBefore(const Pixeld& inPix) const = 0;
    virtual Pixeld adaptAfter(const Pixeld& inPix) const = 0;
    virtual ~GeoRefAdapter() {}
};

class FactorAdapter {
public:
    FactorAdapter(std::vector<double> &offsets, double fac);
    Pixeld adaptBefore(const Pixeld& inPix) const;
    Pixeld adaptAfter(const Pixeld& inPix) const;
private:
    std::vector<double> _offsets;
    double _factor;
};

class ScaleRotateAdapter {
public:
    ScaleRotateAdapter(const std::vector<double>& pivot, const std::vector<double>& targetOffet, double angle, double scale);
    Pixeld adaptBefore(const Pixeld& inPix) const;
    Pixeld adaptAfter(const Pixeld& inPix) const;
private:
    std::vector<double> _pivot;
    std::vector<double> _offset;
    double _angle;
    double _scale;
};
}

#endif // GEOREFADAPTER_H
