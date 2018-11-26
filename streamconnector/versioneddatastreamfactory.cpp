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
#include "abstractfactory.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "streamconnector.h"
#include "versionedserializer.h"
#include "versioneddatastreamfactory.h"



using namespace Ilwis;
using namespace Stream;

std::map<StreamerKey, CreateStreamIO, LessStreamKey> VersionedDataStreamFactory::_dataStreamers;

bool LessStreamKey::operator()(const StreamerKey &val1, const StreamerKey &val2) const
{
    if ( hasType(val1._type,val2._type) && val1._version <= val2._version)
        return false;

    return val1._type < val2._type && val1._version <= val2._version;
}


VersionedDataStreamFactory::VersionedDataStreamFactory() : AbstractFactory("VersionedDataStreamFactory","ilwis")
{
}

VersionedSerializer *VersionedDataStreamFactory::create(const QString &version,IlwisTypes tp, QDataStream& stream)
{
    if ( version == "")
        return 0;

    StreamerKey key(version, tp)    ;

    auto iter =  _dataStreamers.find(key);
    if ( iter != _dataStreamers.end())
        return ((*iter).second)(stream);
    return 0;
}

void VersionedDataStreamFactory::addCreator(const StreamerKey &key, CreateStreamIO streamer)
{
   auto iter =  _dataStreamers.find(key);
   if ( iter == _dataStreamers.end())
       _dataStreamers[key] = streamer;
}


