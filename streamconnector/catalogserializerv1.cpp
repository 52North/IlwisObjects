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
#include "factory.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "catalog.h"
#include "oshelper.h"
#include "catalogserializerv1.h"

using namespace Ilwis;
using namespace Stream;

CatalogserializerV1::CatalogserializerV1(QDataStream &stream, const QString &version) : VersionedSerializer(stream, version)
{
}

bool CatalogserializerV1::loadItems(std::vector<Resource>& items ) {
    if (!VersionedSerializer::loadItems(items))
        return false;

    quint32 sz;
    _stream >> sz;
    for(int i = 0; i < sz; ++i){
        Resource resource;
        resource.load(_stream);
        items.push_back(resource);
    }
    return true;
}

bool CatalogserializerV1::store(IlwisObject *obj, const Ilwis::IOOptions &options)
{
    if (!VersionedSerializer::store(obj, options))
        return false;

    Catalog *catalog = static_cast<Catalog *>(obj);
    catalog->prepare();
    QString baseurl;
    if (!options.contains("baseurl"))
        return false;
    baseurl = options["baseurl"].toString();



    std::vector<Resource> items;
    if ( options.contains("data"))
        items = catalog->items();

    if ( options.contains("operations")){
        ICatalog cat("ilwis://operations");
        std::vector<Resource> operations = cat->items();
        std::copy(operations.begin(), operations.end(), std::back_inserter(items));
    }

    _stream << (quint32)items.size();

    for(Resource resource : items){
        adaptResource(baseurl, resource);
        resource.store(_stream);

    }
    _stream << catalog->parentCatalog();

    return true;


}

void CatalogserializerV1::adaptProperyResource(const QString& baseUrl, Resource& resource, const QString& propertyName) const
{
    Resource properyResource = resource.property2Resource(propertyName, itCOORDSYSTEM);
    if ( properyResource.isValid()){
        QString url = adaptedUrl(baseUrl, properyResource,true);
        resource.addProperty(propertyName, url);
    }
}

void CatalogserializerV1::adaptResource(const QString& baseUrl, Resource& resource) const{

    QString tempName = resource.name();
    resource.setUrl(adaptedUrl(baseUrl, resource, false));
    resource.setUrl(adaptedUrl(baseUrl, resource, true), true);
//    QString container = QString(baseUrl).arg(resource.ilwisType() == itOPERATIONMETADATA ? "operation" : "data").arg("catalog");
//    resource.addContainer(QUrl(container));
    resource.name(tempName, false);
    adaptProperyResource(baseUrl, resource,"coordinatesystem");
    adaptProperyResource(baseUrl, resource,"georeference");
    adaptProperyResource(baseUrl, resource,"domain");
}

QString CatalogserializerV1::adaptedUrl(const QString& baseUrl, const Resource& resource, bool asRaw) const{
    QString url;
    if ( asRaw){
        QString sourceurl = resource.url(true).toString();
        QString tail = sourceurl.mid(sourceurl.lastIndexOf("/") + 1);
        url = QString(baseUrl).arg(tail).arg(IlwisObject::type2Name(resource.ilwisType()).toLower());
        if ( resource.ilwisType() == itOPERATIONMETADATA){
            url = url.replace("/dataaccess?","/operation?");
            url = url.replace("?datasource=","?operationcode=");
        }
    }else {
        QUrl burl(baseUrl);
        QString host = burl.host();
        int port = burl.port();
        QString path = resource.ilwisType() == itOPERATIONMETADATA ? "" : burl.path();
        url = QString("http://%1:%2%3/%4").arg(host).arg(port).arg(path).arg(resource.name());
    }
    return url;
}

bool CatalogserializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if ( hasType(obj->resource().extendedType(), itRASTER)){
        _stream.device()->seek(0); // reset the position for next reads
        return true;
    }
    std::vector<Resource> items;
    if(!loadItems(items))
        return false;

    mastercatalog()->addItems(items);

    QUrl url;
    _stream >> url;
    Catalog *catalog = static_cast<Catalog *>(obj);
    catalog->setParentCatalog(QUrl(url));

    return true;
}

VersionedSerializer *CatalogserializerV1::create(QDataStream &stream, const QString &version)
{
        return new CatalogserializerV1(stream, version);
}
