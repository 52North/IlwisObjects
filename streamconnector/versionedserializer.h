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

#ifndef VERSIONEDSERIALIZER_H
#define VERSIONEDSERIALIZER_H

namespace Ilwis {
class DataDefinition;

namespace Stream {

class StreamConnector;
class RawConverter;

class VersionedSerializer : public DataInterface
{
public:
    VersionedSerializer(QDataStream &stream);

    virtual ~VersionedSerializer();

    bool loadMetaData(IlwisObject*obj, const IOOptions & );
    bool loadData(IlwisObject*, const IOOptions& options = IOOptions()) { return false; }
    virtual bool loadItems(std::vector<Resource>& ) ;
    bool dataIsLoaded() const ;
    bool store(IlwisObject *obj, const IOOptions& options = IOOptions() );
    virtual bool storeData(IlwisObject *obj, const IOOptions& options = IOOptions() ) { return true;} // not mandatory to implement this, so empty is ok
    virtual quint32 loadGridBlock(IlwisObject *data, quint32 block, QByteArray &blockdata, const RawConverter& converter, const IOOptions &) { return 0;}
    void connector(StreamConnector *streamconnector);
    qint64 beginDataSection() const;

protected:
    QDataStream& _stream;
    StreamConnector *_streamconnector = 0;
    bool _dataLoaded = false;

    bool storeDataDefintion(const DataDefinition& def, QDataStream& stream,  const IOOptions& options = IOOptions()) const;
    bool loadDataDefinition(DataDefinition& def, QDataStream& stream, const Ilwis::IOOptions &options);
    void storeSystemPath(const Resource& resource) const;
    template<class T> T makeSystemObject(const QString& url){
        T systemObject;
        if ( url.indexOf("ilwis://system") == 0){ // it used to be a system object, check if it still is and if so use it
            systemObject.prepare(url,{"mustexist", true});
        }
        return systemObject;
    }

    bool store(const QString &v, IlwisTypes valueType, const Ilwis::IOOptions &options);
    bool loadMetaData(const Ilwis::IOOptions &options, IlwisTypes tp, QString &v);
};
}
}

#endif // STREAMCONNECTORV1_H
