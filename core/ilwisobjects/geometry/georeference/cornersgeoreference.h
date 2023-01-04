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

#ifndef CORNERSGEOREFERENCE_H
#define CORNERSGEOREFERENCE_H

#include "kernel_global.h"

namespace Ilwis {

class GeoRefImplementation;

class KERNELSHARED_EXPORT CornersGeoReference : public SimpelGeoReference
{
public:
    CornersGeoReference();
    CornersGeoReference(const Resource& resource);
	void internalEnvelope(const Envelope& env);
    int compute();
    QSize computeGridSize() const;
    bool isCornersOfCorners() const;
    Envelope internalEnvelope() const;
    GeoRefImplementation *clone();
    static QString typeName();
    bool isCompatible(const IlwisData<GeoReference>& georefOther) const;
	QString grfType() const;


    static GeoRefImplementation *create();
    QString toCode() const;
private:
    void copyTo(GeoRefImplementation *impl);
    Envelope _internalEnvelope;
};

typedef IlwisData<CornersGeoReference> ICornersGeoReference;
}

#endif // CORNERSGEOREFERENCE_H
