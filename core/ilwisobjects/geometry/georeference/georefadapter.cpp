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

#include "kernel.h"
#include "geometries.h"
#include "georefadapter.h"

using namespace Ilwis;

GeoRefAdapter::GeoRefAdapter()
{
}

//-------------------------------------------------
FactorAdapter::FactorAdapter(std::vector<double>& offsets, double fac) :
    _offsets(offsets),_factor(fac)
{
}

Pixeld FactorAdapter::adaptBefore(const Pixeld &inPix) const
{
    Pixeld outPix = inPix / _factor;
    return outPix + _offsets;
}

Pixeld FactorAdapter::adaptAfter(const Pixeld& inPix) const
{
    Pixeld outPix = inPix - _offsets;
    return outPix * _factor;
}

//--------------------------------------------------


ScaleRotateAdapter::ScaleRotateAdapter(const std::vector<double> &pivot, const std::vector<double> &targetOffet, double angle, double scale) :
    _pivot(pivot), _offset(targetOffet), _angle(angle), _scale(scale)
{
}

Pixeld ScaleRotateAdapter::adaptBefore(const Pixeld &inPix) const
{
    Pixeld pix = inPix - _offset;
    double y = cos(_angle) / _scale * pix.y - sin(_angle) / _scale * pix.x;
    double x = sin(_angle) / _scale * pix.y - cos(_angle) / _scale * pix.x;

    return Pixeld(x,y)  + _offset;

}


Pixeld ScaleRotateAdapter::adaptAfter(const Pixeld &inPix) const
{
    Pixeld pixTemp = inPix - _pivot;
    double x = -sin(_angle) * _scale * pixTemp.y + cos(_angle) * _scale * pixTemp.x;
    double y = cos(_angle) * _scale * pixTemp.y + sin(_angle) * _scale * pixTemp.x;

    return Pixeld(x,y) + _offset;

}

//----------------------------------------------------

