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

#ifndef StreamConnector_H
#define StreamConnector_H

#include <QNetworkAccessManager>

namespace Ilwis {
namespace Stream {

class VersionedSerializer;

class StreamConnector : public IlwisObjectConnector
{
public:
    StreamConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    virtual ~StreamConnector();

    bool loadMetaData(IlwisObject* object, const IOOptions&);
    bool loadData(IlwisObject *data, const IOOptions &options);

    QString provider() const;
    quint32 position() const;
    bool needFlush() const;
    void flush(bool last);
    bool isReadOnly() const;
    bool isFileBased() const;
    bool dataIsLoaded() const;
    QString format() const;
    static ConnectorInterface *create(const Resource &resource, bool load, const IOOptions &options);


    qint64 beginDataSection() const;
    void beginDataSection(qint64 begin);
private:
    std::unique_ptr<VersionedSerializer> _versionedConnector;
    std::unique_ptr<QIODevice> _datasource;
    QByteArray _bytes;
    qint64 _beginDataSection = -1;

    bool store(IlwisObject *obj,const IOOptions& options = IOOptions());
    bool openSource(bool reading);
    IlwisObject *create() const;
};

}
}

#endif // StreamConnector_H
