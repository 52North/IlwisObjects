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

#include <QString>

#include "kernel.h"
#include "ilwis.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "georefimplementation.h"

using namespace Ilwis;

GeoRefImplementation::GeoRefImplementation(const QString &type) : Identity(type), _centerOfPixel(true)
{
}

GeoRefImplementation::~GeoRefImplementation()
{
    _csy.set(0);
}

ICoordinateSystem GeoRefImplementation::coordinateSystem() const
{
    return _csy;
}

 void GeoRefImplementation::coordinateSystem(const ICoordinateSystem& csy)
{
    _csy = csy;
}


Size<> GeoRefImplementation::size() const
{
    return _size;
}

void GeoRefImplementation::size(const Size<> &sz)
{
    // size must always be positive or undefined
    if (sz.xsize() > 0 && sz.ysize() > 0)
        _size = sz.toQSize();
}

Envelope GeoRefImplementation::envelope() const
{
    Envelope env;
    if (!_size.isValid())
        return env;
    Coordinate crd = pixel2Coord(Pixeld(0, 0));
    env += crd;
    crd = pixel2Coord(Pixeld(_size.xsize(), 0));
    env += crd;
    crd = pixel2Coord(Pixeld(0, _size.ysize()));
    env += crd;
    crd = pixel2Coord(Pixeld(_size.xsize(), _size.ysize()));
    env += crd;
    return env;
}

void GeoRefImplementation::envelope(const Envelope &)
{

}

bool GeoRefImplementation::centerOfPixel() const
{
    return _centerOfPixel;
}

void GeoRefImplementation::centerOfPixel(bool yesno)
{
    _centerOfPixel = yesno;
}

int GeoRefImplementation::compute()
{
    return 1;
}


bool GeoRefImplementation::isCompatible(const IGeoReference &georefOther) const
{
    return false;
}

void GeoRefImplementation::copyTo(GeoRefImplementation *impl)
{
    impl->_csy = _csy;
    impl->_centerOfPixel = _centerOfPixel;
    impl->_size = _size;
    impl->name( name());
    impl->setDescription(description());
    impl->code(code());
}

bool GeoRefImplementation::isLinear() const
{
  return false;
}
