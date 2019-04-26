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

#ifndef GEOREFERENCE_H
#define GEOREFERENCE_H

#include "kernel_global.h"
#include "georefadapter.h"

namespace Ilwis {

class GeoRefImplementation;
class CoordinateSystem;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;
class GeoRefImplementation;

class GeoRefInterface {
public:
    virtual Coordinate pixel2Coord(const Pixeld&) const = 0;
    virtual Pixeld coord2Pixel(const Coordinate& crd) const = 0;
    virtual double pixelSize() const = 0;
    virtual int compute() = 0;
    virtual bool isLinear() const = 0;
    virtual void coordinateSystem(const ICoordinateSystem& csy)=0;
    virtual Size<> size() const=0;
    virtual void size(const Size<>& sz)=0;
    virtual bool centerOfPixel() const=0;
    virtual void centerOfPixel(bool yesno)=0;
};

class KERNELSHARED_EXPORT GeoReference : public IlwisObject, public GeoRefInterface
{
public:
    GeoReference();
    GeoReference(const Resource& resource);
    ~GeoReference();

    Coordinate pixel2Coord(const Pixeld& pixel) const;
    Pixeld coord2Pixel(const Coordinate& crd) const;
    double pixelSize() const;
    int compute();

    virtual Envelope pixel2Coord(const BoundingBox &box ) const;
    virtual BoundingBox coord2Pixel(const Envelope &box) const;
    ICoordinateSystem coordinateSystem() const;
    void coordinateSystem(const ICoordinateSystem& csy);
    Size<> size() const;
    void size(const Size<>& sz);
    Envelope envelope() const;
    void envelope(const Envelope& env);
    bool centerOfPixel() const;
    void centerOfPixel(bool yesno);
    bool isCompatible(const IlwisData<GeoReference>& georefOther) const;
    bool canUse(const IlwisObject *obj, bool strict=false) const ;
    void adapter(GeoRefAdapter* adapt);
    bool isValid() const;
    virtual bool isLinear() const;
    void impl(GeoRefImplementation *impl);
    template<typename GrfT> QSharedPointer<GrfT> as(){
        QSharedPointer<GrfT> sptr = _georefImpl.dynamicCast<GrfT>();
        if ( sptr.isNull())
            throw ErrorObject(TR(ERR_COULD_NOT_CONVERT_2).arg("georef").arg(GrfT::typeName()));
        return sptr;
    }
    template<typename GrfT> bool grfType() const {
        return dynamic_cast<GrfT *>(_georefImpl.data()) != 0;
    }

	QString grfType() const;
    IlwisObject *clone();
    void copyTo(IlwisObject *obj);

    void create(const QString& type);

    IlwisTypes ilwisType() const;


protected:
    const QScopedPointer<GeoRefAdapter> &adapter() const;
private:
    QSharedPointer<GeoRefImplementation> _georefImpl;
    QScopedPointer<GeoRefAdapter> _adapter;

};

typedef IlwisData<GeoReference> IGeoReference;
}

Q_DECLARE_METATYPE(Ilwis::IGeoReference)

#endif // GEOREFERENCE_H
