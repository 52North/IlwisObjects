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
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "georefimplementation.h"
#include "undeterminedgeoreference.h"

using namespace Ilwis;

GeoRefImplementation *UndeterminedGeoReference::create()
{
    return new UndeterminedGeoReference();
}

QString UndeterminedGeoReference::grfType() const {
	return UndeterminedGeoReference::typeName();
}

QString UndeterminedGeoReference::typeName()
{
    return "undeterminedGeoReference";
}

UndeterminedGeoReference::UndeterminedGeoReference() : GeoRefImplementation("undetermined")
{
}

Coordinate UndeterminedGeoReference::pixel2Coord(const Pixeld &pix) const
{
    if ( !pix.isValid())
        return Coordinate();
    if (pix.is3D())
        return Coordinate(pix.x,  size().ysize() - pix.y, pix.z);
    return Coordinate(pix.x, size().ysize() - pix.y);
}

Pixeld UndeterminedGeoReference::coord2Pixel(const Coordinate &crd) const
{
    if ( !crd.isValid())
        return Pixeld();
    if ( crd.is3D())
        return Pixeld(crd.x, size().ysize() - crd.y, crd.z);
    auto pix =  Pixeld(crd.x, size().ysize() - crd.y);
	if (!isNumericalUndef(crd.z))
		pix.z = pix.z;
	return pix;
}

double UndeterminedGeoReference::pixelSize() const
{
    return rUNDEF;
}

GeoRefImplementation *UndeterminedGeoReference::clone()
{
    return new UndeterminedGeoReference();
}

bool UndeterminedGeoReference::isValid() const {
	return true;
}


