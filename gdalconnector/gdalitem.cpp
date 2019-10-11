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

#include <QString>
#include <QHash>
#include <QUrl>
#include <QLibrary>
#include <QStringList>
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "resource.h"
#include "gdalproxy.h"
#include "gdalitem.h"
#include "proj4parameters.h"
#include "mastercatalog.h"
#include "size.h"
#include "raster.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#include "quazip//quazipdir.h"

using namespace Ilwis;
using namespace Gdal;
GDALItems::GDALItems(const QFileInfo &localContainerFile, IlwisTypes tp, IlwisTypes extTypes) {
	QFileInfo file = localContainerFile;
	GdalHandle* handle = gdal()->openFile(file.absoluteFilePath(), i64UNDEF, GA_ReadOnly, false);
	if (handle) {
		QUrl url = QUrl::fromLocalFile(file.absoluteFilePath());
		quint64 sz = file.size();
		int count = layerCount(handle);
		if (count == 0) {// could be a complex dataset
			handleComplexDataSet(handle->handle());
		}
		else {
			QString code;
			quint64 csyId = addCsy(handle, file.absoluteFilePath(), url, code, false);
			if (handle->type() == GdalHandle::etGDALDatasetH) {
				quint64 grfId = addItem(handle, url, code, 0, itGEOREF, itCOORDSYSTEM);
				//addItem(handle, url, csyId, grfId, itRASTER,itGEOREF | itNUMERICDOMAIN | itCONVENTIONALCOORDSYSTEM,sz);
				for (int i = 0; i < count; ++i) {
					auto layerHandle = gdal()->getRasterBand(handle->handle(), i + 1);
					if (layerHandle) {
						QString layername = file.baseName() + "_" + QString::number(i + 1);
						QString containerUrl = url.toString() + "/" + layername;
						addItem(handle, containerUrl, code, grfId, itRASTER, itGEOREF | itNUMERICDOMAIN | itCONVENTIONALCOORDSYSTEM, sz / count, i, false);
					}
				}
			}
			else {
				if (count == 1) {//  default case, one layer per object
					OGRLayerH layerH = gdal()->getLayer(handle->handle(), 0);
					int featureCount = gdal()->getFeatureCount(layerH, FALSE);
					sz = findSize(file);
					if (tp & itFEATURE) {
						addItem(handle, url, code, featureCount, itFEATURE, itCOORDSYSTEM | itTABLE, sz, iUNDEF, false);
						addItem(handle, url, 0, iUNDEF, itTABLE, itFEATURE, sz);
						if (!mastercatalog()->id2Resource(csyId).isValid())
							addItem(handle, QUrl(url), 0, iUNDEF, itCONVENTIONALCOORDSYSTEM, 0, sz);
					}
					else if (tp & itTABLE)
						addItem(handle, url, 0, iUNDEF, itTABLE, 0, sz, iUNDEF, false);
				}
				else { // multiple layers, the file itself will be marked as container; internal layers will be added using this file as container
					//TODO: atm the assumption is that all gdal containers are files. this is true in the majority of the cases but not in all. Without a proper testcase the non-file option will not(yet) be implemented
					addItem(handle, url, QString::number(count), iUNDEF, itCATALOG, extTypes | itFILE | itFEATURE);
					for (int i = 0; i < count; ++i) {
						OGRLayerH layerH = gdal()->getLayer(handle->handle(), i);
						if (layerH) {
							const char *cname = gdal()->getLayerName(layerH);
							int featureCount = gdal()->getFeatureCount(layerH, FALSE);
							if (cname) {
								QString layerName(cname);
								QString layerurl = url.toString() + "/" + layerName;
								addItem(handle, QUrl(layerurl), code, featureCount, itFEATURE, itCOORDSYSTEM | itTABLE, sz, iUNDEF, false);
								addItem(handle, QUrl(layerurl), 0, iUNDEF, itTABLE, itFEATURE, sz);
								if (!mastercatalog()->id2Resource(csyId).isValid())
									addItem(handle, QUrl(layerurl), 0, iUNDEF, itCONVENTIONALCOORDSYSTEM, 0, sz);
							}
						}
					}
				}
			}
			gdal()->closeFile(file.absoluteFilePath(), i64UNDEF);
		}
	}
}

int GDALItems::layerCount(GdalHandle* handle)
{
	int count = 0;
	if (handle->type() == GdalHandle::etGDALDatasetH) {
		count = gdal()->layerCount(handle->handle());
	}
	else if (handle->type() == GdalHandle::etOGRDataSourceH) {
		count = gdal()->getLayerCount(handle->handle());
	}
	return count;
}

GDALItems::GDALItems(const QUrl &url, const QFileInfo &localFile, IlwisTypes tp, IlwisTypes extTypes)
{
	if (localFile.isRoot())
		return;
	QFileInfo file = localFile;
	GdalHandle* handle = gdal()->openFile(file.absoluteFilePath(), i64UNDEF, GA_ReadOnly, false);
	if (!handle) {
		// try sentinel
		if (file.suffix().toLower() == "zip") {
			if (file.baseName().left(4).toLower() == "s2a_") { // sentinel 2 candidate
				QuaZip zip(file.absoluteFilePath());
				zip.open(QuaZip::Mode::mdUnzip);
				QuaZipDir nav(&zip);
				QStringList filters;
				filters.append("*.xml");
				QList<QuaZipFileInfo64> entries = nav.entryInfoList64();
				//            QList<QuaZipFileInfo> allitems = zip.getFileInfoList();
				int err = zip.getZipError();
				zip.close();
			}
		}
	}
	if (handle) {
		quint64 sz = file.size();
		int count = layerCount(handle);
		if (count == 0) {// could be a complex dataset
			addItem(handle, QUrl::fromLocalFile(file.absoluteFilePath()), QString::number(handleComplexDataSet(handle->handle())), iUNDEF, itCATALOG, itFILE | itRASTER);
			return;
		}
		//TODO: at the moment simplistic approach; all is corners georef and domain value
		// and a homogenous type if files. when we have example of more complex nature we wille xtend this+
		QString code;
		quint64 csyId = addCsy(handle, file.absoluteFilePath(), url, code, false);
		if (handle->type() == GdalHandle::etGDALDatasetH) {
			quint64 grfId = addItem(handle, url, code, 0, itGEOREF, itCOORDSYSTEM);
			if (count == 1)
				addItem(handle, url, code, grfId, itRASTER, itGEOREF | itNUMERICDOMAIN | itCONVENTIONALCOORDSYSTEM, sz, iUNDEF, false);
			else {
				addItem(handle, url, code, grfId, itRASTER, itCATALOG | itGEOREF | itNUMERICDOMAIN | itCONVENTIONALCOORDSYSTEM | itFILE, sz);
				addItem(handle, url, code, iUNDEF, itCATALOG, itFILE | itRASTER);
			}
		}
		else {
			if (count == 1) {//  default case, one layer per object
				OGRLayerH layerH = gdal()->getLayer(handle->handle(), 0);
				int featureCount = gdal()->getFeatureCount(layerH, FALSE);
				IlwisTypes featuretype = GDALProxy::translateOGRType(gdal()->getLayerGeometry(layerH));
				sz = findSize(file);
				if (tp & itFEATURE) {
					addItem(handle, url, code, featureCount, featuretype, itCOORDSYSTEM | itTABLE, sz, iUNDEF, false);
					addItem(handle, url, 0, iUNDEF, itTABLE, itFEATURE, sz);
					if (!mastercatalog()->id2Resource(csyId).isValid())
						addItem(handle, QUrl(url), 0, iUNDEF, itCONVENTIONALCOORDSYSTEM, 0, sz);
				}
				else if (tp & itTABLE)
					addItem(handle, url, 0, iUNDEF, itTABLE, 0, sz);
			}
			else { // multiple layers, the file itself will be marked as container; internal layers will be added using this file as container
				//TODO: atm the assumption is that all gdal containers are files. this is true in the majority of the cases but not in all. Without a proper testcase the non-file option will not(yet) be implemented
				addItem(handle, url, QString::number(count), iUNDEF, itCATALOG, extTypes | itFILE | itFEATURE);
				for (int i = 0; i < count; ++i) {
					OGRLayerH layerH = gdal()->getLayer(handle->handle(), i);
					if (layerH) {
						const char *cname = gdal()->getLayerName(layerH);
						int featureCount = gdal()->getFeatureCount(layerH, FALSE);
						if (cname) {
							QString layerName(cname);
							QString layerurl = url.toString() + "/" + layerName;
							addItem(handle, QUrl(layerurl), code, featureCount, itFEATURE, itCOORDSYSTEM | itTABLE, sz, iUNDEF, false);
							addItem(handle, QUrl(layerurl), 0, iUNDEF, itTABLE, itFEATURE, sz);
							if (!mastercatalog()->id2Resource(csyId).isValid())
								addItem(handle, QUrl(layerurl), 0, iUNDEF, itCONVENTIONALCOORDSYSTEM, 0, sz);
						}
					}
				}
			}
		}
		gdal()->closeFile(file.absoluteFilePath(), i64UNDEF);
	}
}
quint64 GDALItems::findSize(const QFileInfo& inf){
    quint64 size = inf.size();
    if ( inf.suffix().toLower() == "shp"){
        QString path = inf.absolutePath() + "/" + inf.baseName();
        size += QFileInfo(path + ".dbf").size();
        size += QFileInfo(path + ".sbn").size();
        size += QFileInfo(path + ".prj").size();
        size += QFileInfo(path + ".sbx").size();
        size += QFileInfo(path + ".shx").size();
    }
    return size;
}
quint64 GDALItems::caseWithSquareBrackets(const QStringList& parts, Size<>& sz, QString& shortname){
    QString metadata = parts[0];
    QStringList metadataparts = metadata.split("]");
    QString sizestring = metadataparts[0].mid(1);
    QStringList sizeparts = sizestring.split("x");
    sz = getSize(sizeparts);

    // in the brackets case, y comes before x
    sz = Size<>(sz.ysize(), sz.xsize(), sz.zsize());

    return extractNameAndDomain(metadataparts[1], shortname);
}

Size<> GDALItems::getSize(const QStringList& szMembers)
{
    Size<> sz;
    bool ok1, ok2, ok3;
    if ( szMembers.size() == 2){
        int x = szMembers[0].toInt(&ok1);
        int y = szMembers[1].toInt(&ok2);
        if ( ok1 && ok2)
            sz = Size<>(x,y,1);
    } else if ( szMembers.size() == 3){
        int z = szMembers[0].toInt(&ok1);
        int x = szMembers[1].toInt(&ok2);
        int y = szMembers[2].toInt(&ok3);
        if ( ok1 && ok2 && ok3)
            sz = Size<>(x,y,z);
    }
    return sz;
}

quint64 GDALItems::extractNameAndDomain(const QString& part, QString& shortname)
{
    QStringList secondPart = part.split("(");
    shortname = secondPart[0].trimmed();
    QString numbertype = secondPart[1].left(secondPart[1].size() - 1);
    return numbertype2domainid(numbertype);
}

quint64 GDALItems::caseWithurl(const QStringList& parts, Size<>& sz, QString& shortname)
{

    QString sizeString = parts[0].mid(1,parts[0].size() - 3);
    QStringList szMembers = sizeString.split("x");
    sz = getSize(szMembers);
    quint64 domid = extractNameAndDomain(parts[1], shortname);

    return domid;
}

void GDALItems::addOffsetScale(void *handle, const int count, Resource & gdalitem)
{
    GDALRasterBandH band = gdal()->getRasterBand(handle, count+1);
    int ok;
    double offset, scale;
    offset = gdal()->getRasterOffset(band, &ok);
    if (ok)
        gdalitem.addProperty("offset", offset);
    scale = gdal()->getRasterScale(band, &ok);
    if (ok)
        gdalitem.addProperty("scale", scale);
}

int GDALItems::handleComplexDataSet(void *handle){
    char **pdatasets = gdal()->getMetaData(handle, "SUBDATASETS");
    if ( pdatasets == 0)
        return iUNDEF;

    auto datasetdesc = kvp2Map(pdatasets);

    auto iter = datasetdesc.begin();
    // we know there is altijd  pairs SUBDATASET_<n>_DESC,SUBDATASET_<n>_NAME in the map
    int count = 0;
    while(iter !=  datasetdesc.end()) {
        Size<> sz;
        QString shortname;
        quint64 domid        ;
        QStringList parts = iter->second.split("//");
        if ( parts.size() == 2)
            domid = caseWithurl(parts, sz, shortname);
        else {
            if ( iter->second[0] == '[' && iter->second.lastIndexOf(']') > 1){
                domid = caseWithSquareBrackets(parts,sz, shortname);
            }
        }
        ++iter;
        QString encodedUrl = QUrl::toPercentEncoding(iter->second,"/","\"");
        QString rawUrl = "gdal://"+ encodedUrl;
        parts = iter->second.split("\"");
		if (parts.size() < 2) // unknow structure of url
			return iUNDEF;
        QString normalizedUrl = OSHelper::createFileUrlFromParts(parts[1] , "/" + shortname);
        Resource gdalitem(normalizedUrl,rawUrl,itRASTER);
        gdalitem.code(iter->second);
        gdalitem.name(shortname, false);
        gdalitem.dimensions(sz.toString());
        GDALDatasetH handle = gdal()->open(gdalitem.code().toLocal8Bit(), GA_ReadOnly);
        GdalHandle ihandle(handle,GdalHandle::etGDALDatasetH);
		QString code;
        addCsy(&ihandle,gdalitem.code(),normalizedUrl,code, false);
        if ( code != sUNDEF){
            gdalitem.addProperty("coordinatesystem", code,true);
        }
        addOffsetScale(handle, count, gdalitem);
        gdal()->close(handle);
        gdalitem.addProperty("domain",domid);
        insert(gdalitem);
        ++iter;
        ++count;

    }
    return count;
}

quint64 GDALItems::numbertype2domainid(const QString& numbertype) const{
    QString systemdomain="value";
    if ( numbertype.indexOf("integer") != -1)
        systemdomain = "integer";
    if ( numbertype == "16-bit integer")
        systemdomain = "integer";
    if ( numbertype == "8-bit integer"){
            systemdomain = "image";
    } if ( numbertype == "16-bit unsigned integer"){
        systemdomain = "image16"    ;
    } else if ( numbertype == "32-bit unsigned integer"){
        systemdomain = "count";
    }
    IDomain dom(systemdomain);
    return dom->id();
}

QString GDALItems::dimensions(GdalHandle* handle, bool & is3d, int layerindex) const
{
    Size<> sz(gdal()->xsize(handle->handle()), gdal()->ysize(handle->handle()), gdal()->layerCount(handle->handle()));
    QString dim = QString("%1 %2").arg(sz.xsize()).arg(sz.ysize());
    if ( sz.zsize() != 1 && layerindex == iUNDEF){
        dim += " " + QString::number(sz.zsize());
        is3d = true;
    }

    return dim;
}

quint64 GDALItems::addItem(GdalHandle* handle, const QUrl& url, const QString csyCode, quint64 grfId, IlwisTypes tp, IlwisTypes extTypes, quint64 sz, int layerindex, bool isExtendedType) {
    if ( csyCode == sUNDEF)
        return i64UNDEF;

    Resource gdalItem(url, tp);
    if ( sz != i64UNDEF)
        gdalItem.size(sz);
    bool is3D = false;
	if (!hasType(tp, itCATALOG)) {
		bool ok;
		csyCode.toInt(&ok);
		if (!ok)
			gdalItem.addProperty("coordinatesystem", csyCode, true);
	}
    if ( tp == itFEATURE){
		QString count = grfId == -1 ? "" : QString::number(grfId);
        gdalItem.dimensions(count);// misuse of grfid
    }
    else if ( tp == itRASTER){
        Resource resValue = mastercatalog()->name2Resource("code=domain:value",itNUMERICDOMAIN);
        gdalItem.addProperty("domain", resValue.id());
		IGeoReference grf;
		if ( grf.prepare(grfId))
			gdalItem.addProperty("georeference", grf->resourceRef().url().toString(),true);

        QString dim = dimensions(handle, is3D, layerindex);
        gdalItem.dimensions(dim);

    } else if (hasType(tp, itGEOREF)){
        gdalItem.dimensions(dimensions(handle, is3D));
    }else{
        if ( tp == itCATALOG){
            gdalItem.dimensions(csyCode); // misuse of csyid :
        }
    }
    gdalItem.setExtendedType(extTypes);
    if ( layerindex != iUNDEF)
        gdalItem.addProperty("bandindex", layerindex);
    if ( isExtendedType)
        gdalItem.addProperty("extendedtype", isExtendedType);

    insert(gdalItem);

    return gdalItem.id();
}

quint64 GDALItems::addCsy(GdalHandle* handle, const QString &path, const QUrl& url, QString& code, bool message) {
	code = sUNDEF;
    quint64 ret = i64UNDEF;
    OGRSpatialReferenceH srshandle = gdal()->srsHandle(handle, path, message);
    if (srshandle != 0){
        const char * projcs_epsg = gdal()->getAuthorityCode(srshandle, "PROJCS");
        const char * geocs_epsg = gdal()->getAuthorityCode(srshandle, "GEOGCS");
        const char * localcs_epsg = gdal()->getAuthorityCode(srshandle, "LOCAL_CS");
        if (projcs_epsg && QString(gdal()->getAuthorityName(srshandle, "PROJCS")).compare("EPSG", Qt::CaseInsensitive) == 0) {
            Resource resource = mastercatalog()->name2Resource(QString("code=epsg:%1").arg(projcs_epsg), itCONVENTIONALCOORDSYSTEM);
			if (resource.isValid()) {
				Resource res(url, itCONVENTIONALCOORDSYSTEM);
				res.code(QString("epsg:%1").arg(projcs_epsg));
				res.addProperty("extendedtype", true);
				insert(res);
				ret = res.id();
				code = "code=" + res.code();
			}
        }else if (geocs_epsg && QString(gdal()->getAuthorityName(srshandle, "GEOGCS")).compare("EPSG", Qt::CaseInsensitive) == 0){
            Resource resource = mastercatalog()->name2Resource(QString("code=epsg:%1").arg(geocs_epsg), itCONVENTIONALCOORDSYSTEM);
			if (resource.isValid()) {
				Resource res(url, itCONVENTIONALCOORDSYSTEM);
				res.code(QString("epsg:%1").arg(projcs_epsg));
				res.addProperty("extendedtype", true);
				insert(res);
				ret = res.id();
				code = "code=" + res.code();
			}
        }else if (localcs_epsg && QString(gdal()->getAuthorityName(srshandle, "LOCAL_CS")).compare("EPSG", Qt::CaseInsensitive) == 0){
            Resource resource = mastercatalog()->name2Resource(QString("code=epsg:%1").arg(localcs_epsg), itCONVENTIONALCOORDSYSTEM);
			if (resource.isValid()) {
				Resource res(url, itCONVENTIONALCOORDSYSTEM);
				res.code(QString("epsg:%1").arg(projcs_epsg));
				res.addProperty("extendedtype", true);
				insert(res);
				ret = res.id();
				code = "code=" + res.code();
			}
        }else {
            char *proj4;
            OGRErr err =  gdal()->export2Proj4(srshandle, &proj4);

            if ( err == OGRERR_NONE && proj4){
                QString sproj4 = proj4;
                gdal()->free(proj4);

                if ( sproj4 != ""){
                    Proj4Def def = Proj4Parameters::lookupDefintion(sproj4);
                    if ( def._epsg != sUNDEF){
                        return mastercatalog()->name2id("code=" + def._epsg);
                    }else {
                        Resource res("code=proj4:" + sproj4, itCONVENTIONALCOORDSYSTEM);
                        QFileInfo inf(path);
                        res.name(inf.fileName());
                        if ( inf.exists()){
                            res.setUrl(QUrl::fromLocalFile(path), true);
                            res.setUrl(QUrl::fromLocalFile(path));
                        }
                        res.addProperty("extendedtype",true);
                        mastercatalog()->addItems({res});
                        insert(res);
						code = "code=" + res.code();
                        return res.id();
                    }
                }else {
                    char *wkt;
                    OGRErr err =  gdal()->exportToWkt(srshandle, &wkt);
                    if ( err == OGRERR_NONE && wkt){
                        QString swkt = wkt;
                        QStringList parts = swkt.split("[");
                        if ( parts.size()>2){
                            QStringList parts2 = parts[1].split(",");
                            if ( parts2.size() > 0){
                                QString name = parts2[0];
                                Proj4Def def = Proj4Parameters::lookupByName(name);
                                if ( def._epsg != sUNDEF){
									code = "code=" + def._epsg;
                                    return mastercatalog()->name2id("code=" + def._epsg);
                                }
                            }
                        }
                        //TODO parse wkt
                    }
                }
            }



        }
    }
    if ( srshandle)
        gdal()->releaseSrsHandle(handle, srshandle, path);

    if(ret == i64UNDEF){
        Resource resource("code=csy:unknown",itCOORDSYSTEM);
        Envelope env = gdal()->envelope(handle,0);
        if ( env.isValid() && !env.isNull()){
            QString dim = QString("%1 x %2 x %3 x %4").arg(env.min_corner().x).arg(env.min_corner().y).arg(env.max_corner().x).arg(env.max_corner().y);
			code = "code=csy:" + env.toString();
            resource.dimensions(dim);

        }
        insert(resource);
        return resource.id();
    }else
        return ret;
}

std::map<QString, QString> GDALItems::kvp2Map(char **kvplist)
{
    std::map<QString, QString> result;
    if ( kvplist != 0){

        quint32 nItems=0;

        while(*(kvplist + nItems) != NULL)
        {
            nItems++;
            //kvplist++;
        }


        for(int i =0; i < nItems; ++i){
            QString item(kvplist[i]);
            QStringList kvp = item.split("=");
            if ( kvp.size() == 2)
                result[kvp[0]] = kvp[1];

        }
    }
    return result;
}




