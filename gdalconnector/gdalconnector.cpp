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
#include "ilwisdata.h"
#include "gdalproxy.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "dataformat.h"
#include "gdalconnector.h"
#include "gdalitem.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "dataformat.h"

using namespace Ilwis;
using namespace Gdal;

GdalConnector::GdalConnector(const Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource,load, options), _internalPath(sUNDEF)
{
    _handle = NULL;
    if ( resource.url(true).hasFragment())
        _internalPath = resource.url(true).fragment();
    if ( resource.code() != "") // if a code is set the code needs to be used to open the file (thatś the role of codes). This is only true (for the moment) for complex files, e.g. HDF like structures.
        _code = resource.code();
    _fileUrl = resource.url(true);
	if (options.contains("prefix")) {
		_prefix = options["prefix"].toString();
	}
}

GdalConnector::~GdalConnector()
{
//    if ( _handle)
//        delete _handle;
    //TODO: delete causes a crash for unknown reasons; research later
}


IlwisTypes GdalConnector::ilwisType(const QString &name)
{
    if (name == sUNDEF)
        return itUNKNOWN;

    QString filename = name;
    if (name.contains("?")) {
        filename = name.split("?").front();
    }
    QFileInfo inf(filename);
    if ( inf.isDir())
        return itCATALOG;

    QString ext = inf.suffix();
    QString filter = "*." + ext;
    if (gdal()->getExtensions(itTABLE).contains(filter, Qt::CaseInsensitive))
        return itTABLE;

    if (gdal()->getExtensions(itFEATURE).contains(filter, Qt::CaseInsensitive))
		return itFEATURE | itCOORDSYSTEM | itTABLE;

    if ( gdal()->getExtensions(itRASTER).contains(filter,Qt::CaseInsensitive))
        return itRASTER | itGEOREF | itCOORDSYSTEM;


    filename = name;
    if ( !name.contains(QRegExp("\\\\|/"))){
        if ( context()->workingCatalog().isValid() )
            filename = context()->workingCatalog()->filesystemLocation().toLocalFile() + "/" + name;
    }
    if ( gdal()->identifyDriver(filename.toLocal8Bit(), 0) != 0)
        return itRASTER;

    return itUNKNOWN; //TODO: add table formats here
}

bool GdalConnector::getHandle(IlwisObject *data) {
	if (_handle == 0) {
		if (!gdal()->isValid()) {
			return ERROR1(ERR_NO_INITIALIZED_1, "gdal library");
		}
        if ( _code != sUNDEF){
            _handle = gdal()->openFile(_code, data->id(), GA_ReadOnly, false);

        }else {
            if (!_fileUrl.isValid()) {
                return ERROR1(ERR_MISSING_DATA_FILE_1, "Gdal reading");
            }

            QFileInfo fileinf(_fileUrl.toLocalFile());
            QString filen = fileinf.absoluteFilePath();
            if (_prefix != "") {

                filen = _prefix + fileinf.fileName();
            }
            _handle = gdal()->openFile(filen, data->id(), GA_ReadOnly, false); // no messages here
        }
	}
	return true;
}
bool GdalConnector::loadMetaData(IlwisObject *data, const IOOptions &options){
    if (data == nullptr)
        return false;

	getHandle(data);
	QFileInfo fileinf(_fileUrl.toLocalFile());

    if (!_handle){ // could be a container based object
        QString code = sourceRef().code();
        if ( code != sUNDEF){
            _handle = gdal()->openUrl(code,data->id(), GA_ReadOnly,false);
            data->name(sourceRef().name());
        } else {
            // scan our container (parent) and insert it into the mastercatalog
            QFileInfo file = fileinf.absolutePath(); // strip-off the /subdataset at the end, and check if it is a file
            if ( file.exists() && !file.isDir() ) {
                bool prev = kernel()->issues()->silent();
                kernel()->issues()->silent(true); // error messages during scan are not needed
                std::multimap<QString, DataFormat>  formats = DataFormat::getSelectedBy(DataFormat::fpEXTENSION, QString("connector='gdal' and extension = '%1'").arg(file.suffix()));
                IlwisTypes tp;
                IlwisTypes extendedTypes; // from GDALItems: IlwisTypes extendedTypes = extendedType(formats, file.suffix());
                getTypes(formats, tp, extendedTypes);
                GDALItems gdalitems(QUrl::fromLocalFile(file.absoluteFilePath()), file, tp, extendedTypes);
                std::vector<Resource> items;
                for( const auto& resource : gdalitems){
                    items.push_back(resource);
                }
                mastercatalog()->addItems(items);
                kernel()->issues()->silent(prev);
                // read ourselves back out of the mastercatalog, and overwrite our resource
                tp = data->ilwisType();
                auto resource = mastercatalog()->name2Resource(_fileUrl.toString(),tp );
                if (resource.isValid()) {
                    sourceRef() = resource;
                    code = sourceRef().code();
                }
            }
            if ( code != sUNDEF) { // re-try with new code
                _handle = gdal()->openUrl(code,data->id(), GA_ReadOnly,false);
                data->name(sourceRef().name());
            } else { // last resort: attempt opening the parent container, but this will probably fail; if we reached here it is because we encountered a new type gdal-container-file, which is not handled correctly in GDALItems
                _handle = gdal()->openFile(file.absoluteFilePath(), data->id(), GA_ReadOnly);
                data->name(fileinf.fileName());
            }
        }

    } else{
        data->name(fileinf.fileName());
    }
    if (!_handle){
        return ERROR2(ERR_COULD_NOT_OPEN_READING_2,_fileUrl.toString(),QString(gdal()->getLastErrorMsg()));
    }

    if ( hasType(data->ilwisType(), itRASTER|itGEOREF)){
        GDALDriverH driverH = gdal()->getDriverByDataSet(_handle->handle());
        if ( driverH){
            _gdalShortName = gdal()->getShortName(driverH);
        }
    }else if ( hasType( data->ilwisType(), itFEATURE)){
		auto proxy = gdal();
		auto func = proxy->getDriverFromDS;
		GDALDriverH driverH = func(_handle->handle());
        if ( driverH){
            _gdalShortName = gdal()->getDriverName(driverH);
        }
    }

    QVariantList values = DataFormat::getFormatProperties(DataFormat::fpREADWRITE,itCOVERAGE,"gdal", _gdalShortName);
    if ( values.size() == 1){
        _readOnly = values[0].toString().indexOf("c")== -1;
    }
    Time tm = fileinf.created();
    data->createTime(tm);
    tm = fileinf.lastModified();
    data->modifiedTime(tm);


    return true;
}

QString GdalConnector::provider() const
{
    return "gdal";
}

void GdalConnector::format(const QString &f, const IOOptions& op)
{
    _gdalShortName = f;

    QVariantList values = DataFormat::getFormatProperties(DataFormat::fpEXTENSION,itCOVERAGE,"gdal", _gdalShortName);
    if ( values.size() > 0 && _fileUrl.toString().indexOf(values[0].toString())){
        QString ext = "." + values[0].toString();
        QString path = _fileUrl.toString();
        if ( path.toLower().indexOf(ext) == -1){
            path +=  ext;
            // TODO: this might not be correct with hdf based formats
            _fileUrl = path;
            sourceRef().setUrl(path);
            sourceRef().setUrl(path, true);
        }

    }
}

QString GdalConnector::format() const
{
    return _gdalShortName;
}

GDALDataType GdalConnector::ilwisType2GdalType(IlwisTypes tp) {
    switch(tp) {
    case itINT8:
    case itUINT8:
    case itCONTINUOUSCOLOR:
    case itPALETTECOLOR:
        return GDT_Byte;
    case itINT16:
        return GDT_Int16;
    case itUINT16:
        return GDT_UInt16;
    case itINT32:
        return GDT_Int32;
    case itUINT32:
        return GDT_UInt32;
    case itFLOAT:
        return GDT_Float32;
    case itDOUBLE:
        return GDT_Float64;
    default:
        return GDT_Float64;
    }
}

OGRFieldType GdalConnector::ilwisType2GdalFieldType(IlwisTypes tp) {
    if ( hasType(tp, itINTEGER))
        return OFTInteger;
    else if ( hasType(tp, (itFLOAT | itDOUBLE)))
        return OFTReal;
    else if ( tp == itTIME) // no hasType here as itDATETIME also contains itTIME
        return OFTTime;
    else if ( tp == itDATE)
        return OFTDate;
    else if ( tp == itDATETIME)
    if ( tp == itDATETIME)
        return OFTDateTime;
    return OFTString;
}

QString GdalConnector::constructOutputName(GDALDriverH hdriver) const
{
    const char *cext = gdal()->getMetaDataItem(hdriver,GDAL_DMD_EXTENSION,NULL);
    QFileInfo fileinfo =sourceRef().toLocalFile();
    QString filename = fileinfo.absoluteFilePath();
	int index = filename.indexOf(".ilwis4.");
	if (index != -1) {
		filename = filename.remove(".ilwis4");
	}
    if ( cext != 0 ) {
        QString ext(cext);
        int index = filename.lastIndexOf(".");
        if ( index != -1) {
            QString pext = filename.right(filename.size() - index - 1);
            if ( pext.toLower() != ext) {
                filename += "." + ext;
            }
        }else
           filename += "." + ext;
    }
    return filename;
}

OGRLayerH GdalConnector::getLayerHandle() const{
    int layer = 0;
    QFileInfo inf(sourceRef().toLocalFile());
    OGRLayerH hLayer = gdal()->getLayerByName(_handle->handle(), inf.fileName().toLatin1());
    if ( !hLayer) // so this was not a gdal container connector url, then the one layer per source case
         hLayer = gdal()->getLayer(_handle->handle(), layer);
    return hLayer;
}

bool GdalConnector::isReadOnly() const
{
    return _readOnly;
}

void GdalConnector::getTypes(const std::multimap<QString, DataFormat>& formats, IlwisTypes & tp, IlwisTypes & extendedType) const {
    tp = itUNKNOWN;
    extendedType = itUNKNOWN;
    for(auto iter : formats) {
        tp |= iter.second.property(DataFormat::fpDATATYPE).toULongLong();
        extendedType |= iter.second.property(DataFormat::fpEXTENDEDTYPE).toULongLong();
    }
}
