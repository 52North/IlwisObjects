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

#ifndef VERSIONEDDATASTREAMFACTORY_H
#define VERSIONEDDATASTREAMFACTORY_H

namespace Ilwis {
namespace Stream {

typedef std::function<VersionedSerializer *(QDataStream&)> CreateStreamIO;

struct StreamerKey {
    StreamerKey(const QString& version, IlwisTypes tp) : _version(version) {
            _type = hasType(tp, itFEATURE) ? itFEATURE : tp;
    }

    QString _version;
    IlwisTypes _type;
};

struct LessStreamKey
{
  bool operator()(const StreamerKey& val1, const StreamerKey& val2 ) const;
};

class VersionedDataStreamFactory : public AbstractFactory
{
public:
    VersionedDataStreamFactory();

    static VersionedSerializer* create(const QString& version, IlwisTypes tp, QDataStream &stream);
    static void addCreator(const StreamerKey& key, CreateStreamIO streamer);

private:
    static std::map<StreamerKey, CreateStreamIO, LessStreamKey> _dataStreamers;
};

inline bool operator==(const StreamerKey& key1, const StreamerKey& key2){
    return key1._version == key2._version && hasType(key1._type, key2._type);
}
}
}

#endif // VERSIONEDDATASTREAMFACTORY_H
