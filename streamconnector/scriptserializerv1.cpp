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
#include "operationmetadata.h"
#include "script.h"
#include "factory.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "scriptserializerV1.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *ScriptSerializerV1::create(QDataStream& stream)
{
    return new ScriptSerializerV1(stream);
}

ScriptSerializerV1::ScriptSerializerV1(QDataStream& stream) : VersionedSerializer(stream)
{
}

bool ScriptSerializerV1::store(IlwisObject *obj,const IOOptions& options)
{
    if (!VersionedSerializer::store(obj, options))
        return false;
    Script *script = static_cast<Script *>(obj);
    _stream << script->text();
    return true;
}

bool ScriptSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::loadMetaData(obj, options))
        return false;
    QString txt;
    _stream >> txt;
   Script *script = static_cast<Script *>(obj);
   script->text(txt);


    return true;
}

