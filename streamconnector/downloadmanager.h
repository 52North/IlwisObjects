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

#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QDataStream>
#include "ilwis.h"
#include "identity.h"
#include "resource.h"
#include "iooptions.h"
#include "connectorinterface.h"
#include "rawconverter.h"
#include "versionedserializer.h"

namespace Ilwis {

class IlwisObject;
class IOOptions;

namespace Stream {

class VersionedSerializer;


class DownloadManager : public QObject
{
    Q_OBJECT
public:
    DownloadManager(const Ilwis::Resource &resource);
    bool loadMetaData(IlwisObject *object, const IOOptions &options);
    bool loadData(IlwisObject *object, const IOOptions &options);
    std::vector<Resource> loadItems();

protected slots:
    void readReady();
    void readReadyRaster();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void error(QNetworkReply::NetworkError code);
    void finishedMetadata();
    void finishedData();
    void finishedDataExplorer();

private:
    Resource _resource;
    std::unique_ptr<VersionedSerializer> _versionedConnector;
    QByteArray _bytes;
    std::vector<Resource> _items;
    IlwisObject *_object = 0;

    //raster only
    quint32 _blockSizeBytes =0;
    quint32 _currentBlock=0;
    bool _initialRasterData = true;
    RawConverter _converter;

    void copyData(bool lastBlock=false);
};
}
}

#endif // DOWNLOADMANAGER_H
