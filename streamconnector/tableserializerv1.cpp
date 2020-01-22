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
#include "connectorinterface.h"
#include "tableserializerv1.h"
#include "ilwisobjectconnector.h"
#include "streamconnector.h"
#include "factory.h"
#include "abstractfactory.h"
#include "versioneddatastreamfactory.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *TableSerializerV1::create(QDataStream& stream, const QString &version)
{
    return new TableSerializerV1(stream, version);
}

TableSerializerV1::TableSerializerV1(QDataStream& stream, const QString &version) : VersionedSerializer(stream, version)
{
}

bool TableSerializerV1::store(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::store(obj, options))
        return false;
    Table *tbl = static_cast<Table *>(obj);
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;
    _stream << tbl->columnCount();
    _stream << tbl->recordCount();
    for(int col = 0; col < tbl->columnCount(); ++col){
        const ColumnDefinition& coldef = tbl->columndefinitionRef(col);
        _stream << coldef.name();
        _stream << coldef.datadef().domain()->valueType();

        std::unique_ptr<DataInterface> domainStreamer(factory->create(Version::interfaceVersion41, itDOMAIN,_stream));
        if ( !domainStreamer)
            return false;
        storeSystemPath(coldef.datadef().domain()->resource());
        domainStreamer->store(coldef.datadef().domain().ptr(), options);
        if ( !coldef.datadef().range().isNull()) // no range for textdomains
            coldef.datadef().range()->store(_stream);
    }

    return true;
}

bool TableSerializerV1::storeData(IlwisObject *obj, const IOOptions &options ){
    qint64 pos = _stream.device()->pos();
    _stream << pos + sizeof(qint64);
    _stream << itTABLE;
    _stream << Version::interfaceVersion40;
    Table *tbl = static_cast<Table *>(obj);

    std::vector<IlwisTypes> types;
    for(int col =0; col < tbl->columnCount(); ++col){
        types.push_back(tbl->columndefinition(col).datadef().domain()->valueType());
    }

    for(int rec = 0; rec < tbl->recordCount(); ++rec){
        auto record = tbl->record(rec);
        record.storeData(types, _stream,options);
    }
    return true;
}

bool TableSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::loadMetaData(obj, options))
        return false;
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;
    Table *tbl = static_cast<Table *>(obj);
    int columnCount, recordCount;
    std::vector<IlwisTypes> types;
    QString version, url;
    quint64 type;
    _stream >> columnCount;
    _stream >> recordCount;
    tbl->recordCount(recordCount);
    for(int col =0; col < columnCount; ++col){
        QString columnName;
        _stream >> columnName;

        IlwisTypes valueType;
        _stream >> valueType;
        _stream >> url;
        _stream >> type;
        _stream >> version;
        std::unique_ptr<DataInterface> domainStreamer(factory->create(version, itDOMAIN,_stream));
        if ( !domainStreamer)
            return false;
		static_cast<VersionedSerializer *>(domainStreamer.get())->connector(_streamconnector);
        IDomain systemDomain = makeSystemObject<IDomain>(url);
        IDomain dom(type | valueType);
        Range *range = 0;
        types.push_back(valueType);
        domainStreamer->loadMetaData(dom.ptr(), options);
        if ( type != itTEXTDOMAIN){
            range = Range::create(dom->valueType());
            if (!range)
                return false;
            range->load(_stream);
        }

        tbl->addColumn(ColumnDefinition(columnName, systemDomain.isValid() ? systemDomain : dom));
        if ( range)
            tbl->columndefinitionRef(col).datadef().range(range);

    }
    qint64 beginData;
    _stream >> beginData;
    _streamconnector->beginDataSection(obj->ilwisType(), beginData);

    return true;
}

bool TableSerializerV1::loadData(IlwisObject* obj, const IOOptions& options){
    Table *tbl = static_cast<Table *>(obj);
    std::vector<IlwisTypes> types;
    for(int col =0; col < tbl->columnCount(); ++col){
        types.push_back(tbl->columndefinition(col).datadef().domain()->valueType());
    }
    int n = tbl->recordCount();
    tbl->recordCount(0); // we reset the record count as we will be creating the records now which again will up the count
    for(quint32 rec = 0; rec < n; ++rec){
        Record& record = tbl->newRecord();
        record.loadData(types,_stream,options);
    }

    _dataLoaded = true;
    return true;
}
