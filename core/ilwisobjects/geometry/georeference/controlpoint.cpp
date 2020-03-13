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
#include "controlpoint.h"

using namespace Ilwis;

ControlPoint::ControlPoint(bool invalid) : _neverValid(invalid)
{
}

ControlPoint::ControlPoint(const Coordinate &crd, const Pixel &pix) : Coordinate(crd),_gridLocation(pix), _active(false)
{
}

ControlPoint::ControlPoint(const Coordinate &crd, const Pixeld &pix) : Coordinate(crd), _gridLocation(pix), _active(false)
{
}

ControlPoint::ControlPoint(const Coordinate &crd, const LatLon &ll) : Coordinate(crd), _llLocation(ll), _active(false)
{
}

ControlPoint::ControlPoint(const Coordinate &crd, const LatLon &ll, const Pixeld &pix) : Coordinate(crd), _llLocation(ll), _gridLocation(pix), _active(false)
{
}

LatLon ControlPoint::llLocation() const
{
    return _llLocation;
}

void ControlPoint::llLocation(const LatLon &ll)
{
    _llLocation = ll;
}

Pixeld ControlPoint::gridLocation() const
{
    return _gridLocation;
}

Pixeld& ControlPoint::gridLocationRef() 
{
    return _gridLocation;
}

void ControlPoint::gridLocation(const Pixeld &pix)
{
    _gridLocation = pix;
}

bool ControlPoint::isActive() const
{
    return _active;
}

void ControlPoint::active(bool yesno)
{
    _active = yesno;
}

bool ControlPoint::isValid() const
{
    return !_neverValid && Coordinate::isValid() && _gridLocation.isValid();
}

double ControlPoint::errorRow() const {
    return _errorRow;
}
double ControlPoint::errorColumn() const {
    return _errorColumn;
}

void ControlPoint::errorRow(double err) {
    _errorRow = err;
}
void ControlPoint::errorColumn(double err) {
    _errorColumn = err;
}