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
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "coverage.h"
#include "coverageserializerv1.h"
#include "factory.h"
#include "abstractfactory.h"
#include "versioneddatastreamfactory.h"

using namespace Ilwis;
using namespace Stream;

CoverageSerializerV1::CoverageSerializerV1(QDataStream &stream) : VersionedSerializer(stream){

}

bool CoverageSerializerV1::store(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::store(obj, options))
        return false;

    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;
    std::unique_ptr<DataInterface> csyStreamer(factory->create(Version::interfaceVersion, itCOORDSYSTEM,_stream));
    if ( !csyStreamer)
        return false;
    Coverage *coverage = static_cast<Coverage *>(obj);
    storeSystemPath(coverage->coordinateSystem()->resource());
    csyStreamer->store(coverage->coordinateSystem().ptr(),options);

    _stream << coverage->envelope().min_corner().x << coverage->envelope().min_corner().y << coverage->envelope().max_corner().x << coverage->envelope().max_corner().y;

    return true;
}

bool CoverageSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::loadMetaData(obj, options))
        return false;
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    Coverage *coverage = static_cast<Coverage *>(obj);
    quint64 type;
    QString version, url;
    _stream >> url;
    _stream >> type;
    if ( type != itUNKNOWN){

        _stream >> version;

        std::unique_ptr<DataInterface> csyStreamer(factory->create(version, itCOORDSYSTEM,_stream));
        if ( !csyStreamer)
            return false;
        ICoordinateSystem systemCsy = makeSystemObject<ICoordinateSystem>(url);
        ICoordinateSystem csy(type);
        csyStreamer->loadMetaData(csy.ptr(),options);
        coverage->coordinateSystem(systemCsy.isValid() ? systemCsy : csy);
    }

    double minx, miny, maxx, maxy;
    _stream >> minx >> miny >> maxx >> maxy;
    coverage->envelope(Envelope(Coordinate(minx, miny), Coordinate(maxx, maxy)));



    return true;
}



