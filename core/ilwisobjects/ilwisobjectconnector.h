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

#ifndef ILWISOBJECTCONNECTOR_H
#define ILWISOBJECTCONNECTOR_H

#include <QBuffer>
#include "kernel_global.h"
#include "kernel.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "oshelper.h"

namespace Ilwis {
class CatalogConnector;
typedef std::unique_ptr<Ilwis::CatalogConnector> UPCatalogConnector;

class KERNELSHARED_EXPORT IlwisObjectConnector : public QObject, public ConnectorInterface
{
    Q_OBJECT
public:
    IlwisObjectConnector(const Ilwis::Resource &resource, bool,const IOOptions& options=IOOptions() );
    virtual ~IlwisObjectConnector();

    virtual IlwisObject *create() const = 0;
    IlwisTypes type() const;
    Resource& sourceRef() ;
    const Resource& source() const;
    void addProperty(const QString &key, const QVariant &value);
    void removeProperty(const QString &key);
    bool hasProperty(const QString &key) const;
    QVariant getProperty(const QString& ) const;
    const Resource &sourceRef() const;
    bool dataIsLoaded() const;
	void dataIsLoaded(bool yesno);
    void unload();

protected:
    template<class T> T setObject(const QString& propeprty, const QUrl& defaultName) {

        T object;
        bool ok;
        auto id = _resource[propeprty].toLongLong(&ok);
        if (ok){
            Resource resource = mastercatalog()->id2Resource(id);
            if ( resource.code() != sUNDEF)
                ok = object.prepare(id);
        }
        if (!ok)
            object.prepare(OSHelper::neutralizeFileName(defaultName.toString()));

        return object;
    }

    IOOptions ioOptions() const;
    IOOptions& ioOptionsRef();
    template<class T> T setter(const T& resourceValue, const T& dataSourceValue, const T& emptyValue){
        return resourceValue == dataSourceValue || resourceValue == emptyValue ? dataSourceValue : resourceValue;

    }

    Resource _resource;
    bool _binaryIsLoaded;
    std::recursive_mutex _mutex;
    IlwisObject::ConnectorMode _mode = IlwisObject::cmINPUT;

private:
    IOOptions _options;

signals:
    void dataAvailable(QBuffer *buf, bool lastBlock);
	void finishedReadingData();


};
}

#endif // ILWISOBJECTCONNECTOR_H
