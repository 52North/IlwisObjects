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
#include "version.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "factory.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "ellipsoidserializerv1.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *EllipsoidSerializerV1::create(QDataStream& stream)
{
    return new EllipsoidSerializerV1(stream);
}

EllipsoidSerializerV1::EllipsoidSerializerV1(QDataStream& stream) : VersionedSerializer(stream)
{
}

bool EllipsoidSerializerV1::store(IlwisObject *obj,const IOOptions& options)
{
    if (!VersionedSerializer::store(obj, options))
        return false;
    Ellipsoid *ell = static_cast<Ellipsoid *>(obj);
    _stream << ell->majorAxis() << ell->flattening()<< ell->authority();

    return true;
}

bool EllipsoidSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::loadMetaData(obj, options))
        return false;
    double majorAxis, flattening;
    QString authority;
    _stream >> majorAxis >> flattening >> authority;
    Ellipsoid *ellipsoid = static_cast<Ellipsoid *>(obj);
    ellipsoid->setEllipsoid(majorAxis,flattening);
    ellipsoid->setAuthority(authority);


    return true;
}


