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
#include "factory.h"
#include "abstractfactory.h"
#include "georefimplementationfactory.h"

using namespace Ilwis;



GeoReference::GeoReference()
{
}

GeoReference::GeoReference(const Resource& resource) : IlwisObject(resource)
{
}

GeoReference::~GeoReference()
{
//    if ( coordinateSystem().isValid())
//        qDebug() << "use count " << name() << " : " <<  mastercatalog()->usecount(coordinateSystem()->id());

}

void GeoReference::create(const QString& type) {
    GeoRefImplementationFactory *grfFac = kernel()->factory<GeoRefImplementationFactory>("ilwis::georefimplementationfactory");
    GeoRefImplementation *implementation = grfFac->create(type);
    if ( !implementation) {
        ERROR1(ERR_COULDNT_CREATE_OBJECT_FOR_1,type);
    }
    impl(implementation);
}

Coordinate GeoReference::pixel2Coord(const Pixeld &pixel) const
{
    return _georefImpl->pixel2Coord(pixel);
}

Pixeld GeoReference::coord2Pixel(const Coordinate &crd) const
{
    // for performance reasons no isValid check here, haas tobe checked before hand
    return _georefImpl->coord2Pixel(crd);
}

double GeoReference::pixelSize() const
{
    // for performance reasons no isValid check here, haas tobe checked before hand
    return _georefImpl->pixelSize();
}

int GeoReference::compute()
{
    if ( isValid())
        return _georefImpl->compute();
    return false;
}

ICoordinateSystem GeoReference::coordinateSystem() const
{
    if ( isValid())
        return _georefImpl->coordinateSystem();
    return ICoordinateSystem();
}

 void GeoReference::coordinateSystem(const ICoordinateSystem& csy)
{
     if ( isValid() && csy.isValid()){
        _georefImpl->coordinateSystem(csy);
		CoordinateSystem::addCsyProperty(csy, resourceRef());
     }

 }


Size<> GeoReference::size() const
{
    if ( isValid())
        return _georefImpl->size();
    return Size<>();

}

void GeoReference::size(const Size<> &sz)
{
    // size must always be positive or undefined
    if ( isValid() && sz.xsize() > 0 && sz.ysize() > 0)
        _georefImpl->size(sz);
}

Envelope GeoReference::envelope() const
{
    if ( isValid())
        return _georefImpl->envelope();
    return Envelope();
}

bool GeoReference::centerOfPixel() const
{
    if ( isValid())
        return _georefImpl->centerOfPixel();
    return false;
}

void GeoReference::centerOfPixel(bool yesno)
{
    if ( isValid())
        _georefImpl->centerOfPixel(yesno);
}

bool GeoReference::canUse(const IlwisObject *obj, bool strict) const
{
    return true;
}

bool GeoReference::isCompatible(const IGeoReference &georefOther) const
{
    if (!georefOther.isValid())
        return false;

    if (georefOther->id() == id())
        return true;

    if ( !coordinateSystem()->isCompatibleWith(georefOther->coordinateSystem().ptr()))
        return false;

    if ( isValid())
        return _georefImpl->isCompatible(georefOther);

    return false;
}

void GeoReference::adapter(GeoRefAdapter *adapt)
{
    _adapter.reset(adapt);
}

const QScopedPointer<Ilwis::GeoRefAdapter> &GeoReference::adapter() const
{
    return _adapter;
}

Envelope GeoReference::pixel2Coord(const BoundingBox &box) const
{
    if ( !box.isValid()) {
        ERROR2(ERR_INVALID_PROPERTY_FOR_2,"size", "box");
        return Envelope();
    }
    Coordinate c1 = pixel2Coord(box.min_corner());
    Coordinate c2 = pixel2Coord(box.max_corner() + Pixel(1,1));
    return Envelope(c1,c2);
}

BoundingBox GeoReference::coord2Pixel(const Envelope &box) const
{
    if ( !box.isValid()) {
        ERROR2(ERR_INVALID_PROPERTY_FOR_2,"size", "box");
        return  BoundingBox();
    }
    Pixel p1 = coord2Pixel(box.min_corner());
    Pixel p2 = coord2Pixel(box.max_corner());
    return BoundingBox(p1,p2);
}

bool GeoReference::isValid() const
{
	return !_georefImpl.isNull();
}

bool GeoReference::impValid() const {
	return isValid() && _georefImpl->isValid();
}

void GeoReference::impl(GeoRefImplementation *impl)
{
    _georefImpl.reset(impl);
}

QString GeoReference::toString() const
{
    if ( isValid())
        return _georefImpl->toCode() ;

    return sUNDEF;

}

IlwisObject *GeoReference::clone()
{
    if ( !_georefImpl.isNull()) {
        GeoReference *grf = new GeoReference();
        copyTo(grf);
        return grf;
    }
    return 0;
}

void GeoReference::copyTo(IlwisObject *obj)
{
    IlwisObject::copyTo(obj);
    GeoReference *grf = static_cast<GeoReference *>(obj);
    auto * imp = _georefImpl->clone();
    grf->_georefImpl.reset(imp);
}

IlwisTypes GeoReference::ilwisType() const
{
    return itGEOREF;
}

bool GeoReference::isLinear() const
{
    if ( isValid())
        return _georefImpl->isLinear();
    return false;
}

QString GeoReference::grfType() const {
	if (!_georefImpl.isNull()) {
		return _georefImpl->grfType();
	}
	return sUNDEF;
}
