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

#include <QRegExp>
#include <QFileInfo>
#include <QUrl>
#include <QStringList>
#include <QDir>
#include "kernel.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "geometries.h"
#include "identity.h"
#include "catalog/resource.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "odfitem.h"
#include "ilwis3connector.h"

const QString Ilwis::Ilwis3::ODFItem::systemObjectNames="value;image;image;min1to1;nilto1;count;distance;ndvi;percentage;none;latlonwgs84;latlon;bool;byte;color;colorcmp";

using namespace Ilwis;
using namespace Ilwis3;

#include <typeinfo>

ODFItem::ODFItem(const IniFile &file, std::unordered_map<QString, IniFile> *knownInis) : Resource(QUrl::fromLocalFile(file.fileInfo().absoluteFilePath()),itANY),
    _ini(file),
    _knownOdfs(knownInis),
    _projectionName(sUNDEF)
{
    name(_ini.fileInfo().fileName(), false);
    createTime(Time(_ini.fileInfo().created()));
    modifiedTime(Time(_ini.fileInfo().lastModified()));
    addContainer(QUrl::fromLocalFile(_ini.fileInfo().canonicalPath()));
    if (_ini.fileInfo().suffix() == "mpr"){
        QString mpl = _ini.value("Collection","Item0");
        if( mpl != sUNDEF){
            QString internalPath = _ini.fileInfo().canonicalPath() + "/" + mpl;
            if ( QFile::exists(internalPath)) {
                addContainer(QUrl::fromLocalFile(internalPath));
                QUrl newUrl = OSHelper::createFileUrlFromParts(internalPath, "/" + _ini.fileInfo().fileName());
                setUrl(newUrl,false, false); // normalized url
            }
        }
    }
    IlwisTypes csytp, domtp, grftp;
    csytp = domtp = grftp = itUNKNOWN;

    setDescription( _ini.value("Ilwis", "Description"));

    QString path = _ini.fileInfo().absoluteFilePath();
    _ilwtype = Ilwis3Connector::ilwisType(path);
    _csyname = findCsyName(path);
     _domname = findDomainName(path);
    _grfname = findGrfName();
    _datumName = findDatumName();


    csytp = findCsyType(path);
    domtp = findDomainType(path);


    if ( csytp != itUNKNOWN && _ilwtype == itCOORDSYSTEM)
        _ilwtype = csytp;
    if ( domtp != itUNKNOWN && _ilwtype == itDOMAIN)
        _ilwtype = domtp;

    if ( _domname != sUNDEF)
        _extendedType |= domtp;

    if ( _csyname != sUNDEF) {
        _extendedType |= csytp;
        _projectionName = findProjectionName();
        if ( _projectionName != sUNDEF)
            _extendedType |= itPROJECTION;
    }
    if ( _grfname != sUNDEF)
        _extendedType |= itGEOREF;


    if (_datumName != sUNDEF)
        _extendedType |= itGEODETICDATUM;
    if ( _ini.fileInfo().suffix() == "mpl"){
        _isMapList = true;
        _extendedType |= itCATALOG;
    }

   _size = findSize();
    _dimensions = findDimensions();
}

Resource ODFItem::resolveName(const QString& name, IlwisTypes tp) {
    if ( name == sUNDEF)
        return Resource();

    QString filepath = name;
    if ( name.indexOf("/") == -1 && name.indexOf("\\") == -1)    {
        QFileInfo inf(_ini.filepath());
        filepath = inf.absolutePath() + "/" + name;

    }
    Resource res = mastercatalog()->name2Resource(QUrl::fromLocalFile(filepath).toString(), tp);
    if ( res.isValid())
        return res;

    Resource item(filepath, tp);

    return item;
}

std::vector<Resource> ODFItem::resolveNames(){
    std::vector<Resource> resources;
    Resource resource =  resolveName(_domname, itDOMAIN);
    if ( resource.isValid()){
        _properties["domain"] = resource.id();
        resources.push_back(resource);
    }
    resource =  resolveName(_grfname, itGEOREF);
    if ( resource.isValid()){
        _properties["georeference"] = resource.url(true);
        resources.push_back(resource);
    }
    resource =  resolveName(_csyname, itCOORDSYSTEM);
    if ( resource.isValid()){
        _properties["coordinatesystem"] = resource.url(true);
        resources.push_back(resource);
    }
    if ( _datumName != sUNDEF) {
        const Resource& res = mastercatalog()->name2Resource(_datumName,itGEODETICDATUM);
        _properties["geodeticdatum"] = res.id();
    }
    if (_projectionName != sUNDEF) {
        const Resource& res = mastercatalog()->name2Resource(_projectionName,itPROJECTION);
        _properties["projection"] = res.id();
    }

    return resources;
}

bool ODFItem::isMapList() const
{
    return _isMapList;
}

bool ODFItem::resolveNames(const QHash<QString, Resource> &names, const std::map<QString, QString>& fileContainers)
{
    bool ok = true;
	Resource res;
    ok &= setFileId(names, _domname.toLower(), res);
    if ( res.isValid())
        _properties["domain"] = res.url(true);
    ok &= setFileId(names, _grfname.toLower(), res);
    if ( res.isValid())
        _properties["georeference"] = res.url(true);
    ok &= setFileId(names, _csyname.toLower(), res);
    if ( res.isValid())
        _properties["coordinatesystem"] = res.url(true);
    if ( _datumName != sUNDEF) {
        const Resource& resource = mastercatalog()->name2Resource(_datumName,itGEODETICDATUM);
        _properties["geodeticdatum"] = resource.url(true);
    }
    if (_projectionName != sUNDEF) {
        const Resource& resource = mastercatalog()->name2Resource(_projectionName,itPROJECTION);
        _properties["projection"] = resource.url(true);
    }
	if (ilwisType() == itRASTER ) {
		if (container().toString().indexOf(".mpl") == -1) {
			QString loc = OSHelper::neutralizeFileName(url().toString());
			auto iter = fileContainers.find(loc);
			if (iter != fileContainers.end()) {
				QString path = (*iter).second;
				addContainer(path, false);
				//addContainer(path, false);
				setUrl(path + "/" + name());
			}
		}
	}

    return ok;
}

bool ODFItem::setFileId(const QHash<QString, Resource> &names, const QString& value, Resource& outResource) const {
    if ( value == sUNDEF) {
		outResource = Resource();;
        return true; // legal; some properties might not have a value(name)
    }
    if ( Ilwis3Connector::ilwisType(value) & itCOORDSYSTEM) {
        if ( value == "latlonwgs84.csy" ) {
			outResource = mastercatalog()->name2Resource("code=epsg:4326", itCOORDSYSTEM);
            if ( !outResource.isValid()) {
               return ERROR1(ERR_FIND_SYSTEM_OBJECT_1, "Wgs 84");
            }
            return true;
        }
        if ( value == "unknown.csy" ) {
			outResource = mastercatalog()->name2Resource("code=csy:unknown", itCOORDSYSTEM);
            if ( !outResource.isValid()) {
                return ERROR1(ERR_FIND_SYSTEM_OBJECT_1, "'Unknown' coordinate system");
            }
            return true;
        }
    }
    if ( Ilwis3Connector::ilwisType(value) & itGEOREF) {
        if ( value == "none.grf" ) {
			outResource = mastercatalog()->name2Resource("code=georef:undetermined", itGEOREF);
            if ( !outResource.isValid()) {
                return ERROR1(ERR_FIND_SYSTEM_OBJECT_1, "'undetermined' georeference");
            }
            return true;
        }
    }
    QString completeName =  (value.contains(QRegExp("\\\\|/"))) ? value : _ini.fileInfo().canonicalPath() + "/" + value;
    QHash<QString, Resource>::const_iterator iter = names.find(completeName.toLower());
    if (iter != names.end()){
		outResource = iter.value();
    } else {
        // at this time we can't rely on the working catalog to be set(if we are initializing it), so no normal resolve
        // the mastercatalog will contain system items at this moment so we can check these first
        QString baseName = value.left(value.indexOf("."));
        IlwisTypes tp = Ilwis3Connector::ilwisType(value);
		outResource = mastercatalog()->name2Resource(baseName, tp);
        if (!outResource.isValid()) {
            QUrl url = QUrl::fromLocalFile(completeName);
            auto fileid = mastercatalog()->url2id(url, tp);
            if ( fileid == i64UNDEF) {
                kernel()->issues()->log(TR(ERR_MISSING_1).arg(completeName));
                return false;
            }
			outResource = mastercatalog()->id2Resource(fileid);
        }
    }
    return true;

}

QString ODFItem::findProjectionName() const {
   if(_ilwtype & itCONVENTIONALCOORDSYSTEM) {
        QString name = _ini.value("CoordSystem", "Projection");
        return cleanName(name);
   }
   return sUNDEF;
}

QString ODFItem::findDatumName() const {
    if(_ilwtype & itCONVENTIONALCOORDSYSTEM) {
        QString name = _ini.value("CoordSystem", "Datum");
        QString area = _ini.value("CoordSystem", "Datum Area");
        if ( name != sUNDEF) {
            name = Ilwis3Connector::name2Code(name + ( (area!= sUNDEF && area != "") ? "|" + area : ""),"datum");
            return cleanName(name);
        }
    }
    return sUNDEF;
}

QString ODFItem::findDomainName(const QString& path) const
{
    quint64 validTypes = itTABLE | itCOVERAGE | itDOMAIN | itREPRESENTATION;
    if ( (_ilwtype & validTypes) == 0)
        return sUNDEF;

    QString name = sUNDEF;
    if(_ilwtype & itCOVERAGE) {
        QString ext = _ini.fileInfo().suffix().toLower();
        if(ext != "mpl")
            name = _ini.value("BaseMap","Domain");
        else {
            QString rasmap = _ini.value("MapList","Map0");
            QFile file(rasmap) ;
            if ( !file.exists()) {
                rasmap = rasmap.remove("\'");
                rasmap = container(true).toLocalFile() + "/" + rasmap ;
            }
            if ( rasmap.indexOf(".mpr") == -1)
                rasmap += ".mpr";
            IniFile ini;
            getIni(ini, rasmap);
            name = ini.value("BaseMap","Domain");
        }
    }
    else if(_ilwtype & itTABLE){
        name = _ini.value("Table", "Domain");
        if ( name.toLower() == "none.dom")
            return sUNDEF;
    }
    else if(_ilwtype & itDOMAIN)
        name = _ini.fileInfo().fileName();
    else if(_ilwtype & itREPRESENTATION)
        name = _ini.value("Representation", "Domain");
    return cleanName(name);
}

bool ODFItem::getIni(IniFile& ini, const QString& localpath) const{
    if ( _knownOdfs){
        auto iter = (*_knownOdfs).find(localpath.toLower());
        if ( iter != (*_knownOdfs).end()){
            ini = (*iter).second;
        }else if(!ini.setIniFile(localpath))
            return false;

    }else
        if(!ini.setIniFile(localpath))
            return false;
   return true;
}

IlwisTypes ODFItem::findDomainType(const QString& path) const
{
    quint64 validTypes = itTABLE | itCOVERAGE | itDOMAIN | itREPRESENTATION;
    if ( (_ilwtype & validTypes) == 0)
        return itUNKNOWN;

    if  ( _domname == "")
        return itUNKNOWN;

    if ( _domname == "UniqueID")
        return itITEMDOMAIN;
    if ( _domname == "bool.dom")
        return itNUMERICDOMAIN;
    if ( _domname == "value.dom")
        return itNUMERICDOMAIN;
    if ( _domname == "image.dom")
        return itNUMERICDOMAIN;
    if ( _domname == "color.dom")
        return itCOLORDOMAIN;

    IniFile dm;
    QString localpath = container(true).toLocalFile() + "/" + _domname;
    if(!getIni(dm, localpath))
        return itUNKNOWN;


    QString type =  dm.value("Domain", "Type");
    if ( type == "DomainValue")
        return itNUMERICDOMAIN;
    if ( type == "DomainClass" )
        return itITEMDOMAIN;
    if ( type == "DomainIdentifier" || type == "DomainUniqueID")
        return itITEMDOMAIN;
    if ( type == "DomainCOORD" )
        return itCOORDSYSTEM;

    return itUNKNOWN;


}

QString ODFItem::findCsyName(const QString& path) const
{
    quint64 validTypes = itGEOREF | itCOORDSYSTEM | itCOVERAGE;
    if ( (_ilwtype & validTypes) == 0 )
        return sUNDEF;

    QString name = sUNDEF;
    if (_ilwtype & itCOVERAGE) {
        QString ext = _ini.fileInfo().suffix().toLower();
        if(ext != "mpl")
            name = _ini.value("BaseMap","CoordSystem");
        else {
             QString grf = _ini.value("MapList","GeoRef");
             if ( grf != sUNDEF) {
                 grf = grf.remove("\'");
                 QFileInfo infgrf(grf);
                 if ( !infgrf.exists()) {
                     QString grfpath = container().toLocalFile() + "/" + grf;
                     if ( grf != "none.grf"){
                        IniFile ini;
                        getIni(ini, grfpath);
                        name = ini.value("GeoRef","CoordSystem");
                     }else {
                         name = "unknown.csy";
                     }
                 }
             }
        }
    }
    else if ( _ilwtype & itCOORDSYSTEM)
        name = _ini.fileInfo().fileName();
    else if ( _ilwtype & itGEOREF)
        name = _ini.value("GeoRef","CoordSystem");

    return cleanName(name);
}

IlwisTypes ODFItem::findCsyType(const QString& path)
{
    quint64 validTypes = itGEOREF | itCOORDSYSTEM | itCOVERAGE;
    if ( (_ilwtype & validTypes) == 0)
        return itUNKNOWN;

    if ( _csyname == "")
        return itUNKNOWN;

//    Resource resource = mastercatalog()->name2Resource(_csyname,itCOORDSYSTEM);
//    if ( resource.isValid()){
//        return  resource.ilwisType();
//    }
    IniFile csy;
    if ( _csyname == "latlonwgs84.csy"){

        if ( hasType(ilwisType(), itCOVERAGE))
            addProperty("latlonenvelope",_ini.value("BaseMap", "CoordBounds"));

        return itCONVENTIONALCOORDSYSTEM;
    }
    if ( _csyname == "unknown.csy")
        return itBOUNDSONLYCSY;

    QString ext = _ini.fileInfo().suffix().toLower();
    if(ext == "mpl") {
        QString grf = _ini.value("MapList","GeoRef");
        if ( grf != sUNDEF) {
            QString csyname;
            QFileInfo infgrf(grf);
            if ( !infgrf.exists()) {
                grf = grf.remove("\'");
                grf = container().toLocalFile()+ "/" + grf;
            }
            IniFile ini;
            getIni(ini, grf);
            csyname = ini.value("GeoRef","CoordSystem");
            QFile file(csyname);
            if ( !file.exists()){
                csyname = csyname.remove("\'");
                csyname = container(true).toLocalFile() + "/" + csyname;
            }
            getIni(csy, csyname);

        }
    } else {
        QString path = _csyname;
        QFile file(_csyname);
        if ( !file.exists())
            path = container(true).toLocalFile() + "/" + path;
        else{
            QFileInfo inf (_csyname);
            path = inf.absoluteFilePath();
        }

        if(!getIni(csy,path)){
            return itUNKNOWN;
        }
    }

    QString type = csy.value("CoordSystem", "Type");
    if ( type.compare("latlon",Qt::CaseInsensitive) == 0){
        addProperty("projectionname", "LatLon WGS 84");
        return itCONVENTIONALCOORDSYSTEM;
    }
    if ( type.compare("boundsonly",Qt::CaseInsensitive) == 0)
        return itBOUNDSONLYCSY;

    // type empty type can happen due to bug in older version of ilwis. no type was projected
    if ( type == sUNDEF || type.compare("projection",Qt::CaseInsensitive) == 0) {
        QString proj = csy.value("CoordSystem","Projection");
        if ( proj != sUNDEF){
            if ( proj.toLower() == "utm"){
                QString z = csy.value("Projection","Zone");
                proj += " " + z;
            }
            addProperty("projectionname", proj);
        }
        quint64 tp = itCONVENTIONALCOORDSYSTEM ;
        return tp;
    }
    return itUNKNOWN;

}

QString ODFItem::findGrfName() const{
    quint64 validTypes = itGEOREF | itRASTER;
    if ( (_ilwtype & validTypes) == 0)
        return sUNDEF;

    QString name = sUNDEF;

    if ( _ilwtype & itRASTER) {
        name = _ini.value("Map","GeoRef");
        if ( name == sUNDEF)
            name = _ini.value("MapList","GeoRef");
    }
    else if ( _ilwtype & itGEOREF)
        name = _ini.fileInfo().fileName();
    return cleanName(name);

}

quint64 ODFItem::findSize() const
{
    return objectSize();
}

quint64 ODFItem::partSize(const QUrl& file, const QString& section, const QString& key)  const{
    QFileInfo part = file.toLocalFile();
    if ( section != "") {
        IniFile odf;
        odf.setIniFile(file.toLocalFile());
        getIni(odf,file.toLocalFile());
        QString filename = odf.value(section,key);
        if ( !filename.contains(QRegExp("\\\\|/"))) {
            //TODO: changes this for files that are in non folder containers
            filename = part.absolutePath() + "/" + filename;
            part = QFileInfo(filename);
        }

    }
    QFileInfo inf(part.absoluteFilePath());
    return inf.size();

}

quint64 ODFItem::objectSize() const {

    qint64 sz = partSize(_ini.url(),"","");
    bool versionOk;
    double rVersion = _ini.value("Ilwis", "Version").toDouble(&versionOk);

    switch(_ilwtype )
    {
    case itRASTER:
        sz += partSize(_ini.url(), "MapStore", "Data"); break;
    case itTABLE:
        sz += partSize(_ini.url(), "TableStore", "Data"); break;
    case itPOINT:
        sz += partSize(_ini.url(), "TableStore", "Data"); break;
    case itLINE:
        if ( rVersion >= 3.0)
        {
            sz += partSize(_ini.url(), "TableStore", "Data");
            sz += partSize(_ini.url(), "ForeignFormat", "Filename");
        }
        else
        {
            sz += partSize(_ini.url(), "SegmentMapStore", "DataSeg");
            sz += partSize(_ini.url(), "SegmentMapStore", "DataSegCode");
            sz += partSize(_ini.url(), "SegmentMapStore", "DataCrd");
        }
        break;
    case itPOLYGON:
        if ( rVersion >= 3.0)
        {
            sz += partSize(_ini.url(), "top:TableStore", "Data");
            sz += partSize(_ini.url(), "TableStore", "Data");
            sz += partSize(_ini.url(), "ForeignFormat", "Filename");
        }
        else
        {
            sz += partSize(_ini.url(), "SegmentMapStore", "DataSeg");
            sz += partSize(_ini.url(), "SegmentMapStore", "DataCrd");
            sz += partSize(_ini.url(), "PolygonMapStore", "DataPol");
            sz += partSize(_ini.url(), "PolygonMapStore", "DataPolCode");
            sz += partSize(_ini.url(), "PolygonMapStore", "DataTop");
        }
        break;
    case itDOMAIN:
        sz += partSize(_ini.url(), "TableStore", "Data"); break; // only true for Domainssort
    case itGEOREF:
        sz += partSize(_ini.url(), "TableStore", "Data"); break;
    case itCOORDSYSTEM:
        sz += partSize(_ini.url(), "TableStore", "Data"); break;

        if ( _ini.fileInfo().suffix() == "isl")
            sz += partSize(_ini.url(), "Script", "ScriptFile"); break;
        if ( _ini.fileInfo().suffix() == "fun")
            sz += partSize(_ini.url(), "FuncUser", "FuncDeffile"); break;
    }
    return sz;
}

QString ODFItem::findDimensions() const
{
    switch( _ilwtype)
    {
        case itGEOREF:
        {
            QString sX = _ini.value("GeoRef", "Columns");
            if ( sX == "") return "";
            QString sY = _ini.value( "GeoRef", "Lines");
            if ( sY == "") return "";
            return QString("%1 x %2").arg(sX, sY);
        }
        case itRASTER:
        {
            QString sSize = _ini.value( "Map", "Size");
            QStringList xy = sSize.split(" ");
            if ( xy.size() != 2){
                sSize = _ini.value( "MapList", "Size");
                QString zsize = _ini.value( "MapList", "Maps");
                xy = sSize.split(" ");
                if ( xy.size() != 2)
                    return "";
                return QString ("%1 %2 %3").arg( xy[0], xy[1],zsize);
            }

            return QString ("%1 %2").arg( xy[0], xy[1]);
        }
        case itLINE:
            return _ini.value( "SegmentMapStore", "Segments");
        case itPOLYGON:
            return _ini.value( "PolygonMapStore", "Polygons");
        case itPOINT:
            return _ini.value( "PointMap", "Points");
        case itCOORDSYSTEM:
        case itCONVENTIONALCOORDSYSTEM:
        case itBOUNDSONLYCSY:
            {
                QString sV = "";
                QString bnds = _ini.value("CoordSystem", "CoordBounds");
                QStringList parts = bnds.split(" ");
                if ( parts.size() != 4)
                    return "";
                if ( parts[0] == "1e+030" || parts[0] == "-1e+308")
                    return "";

                if ( parts[0].toDouble() > -180 && parts[0].toDouble() < 180)
                    sV = QString("%1 x %2 x %3 x %4").arg(parts[0].toDouble(),0,'g',3).arg(parts[1].toDouble(),0,'g',3).arg(parts[2].toDouble(),0,'g',3).arg(parts[3].toDouble(),0,'g',3);
                else
                    sV = QString("%1 x %2 x %3 x %4").arg(parts[0]).arg(parts[1]).arg(parts[2]).arg(parts[3]);
                return sV;
            }
        case itDOMAIN:
        case itITEMDOMAIN:
            {
                QString dtype =  _ini.value("Domain","Type");
                if ( dtype == "DomainClass" || dtype == "DomainGroup")
                    return _ini.value("DomainClass", "Nr");
                else if ( dtype == "DomainIdentifier" || dtype == "DomainUniqueID")
                    return _ini.value("DomainIdentifier", "Nr");
            }
        case itNUMERICDOMAIN:
            return _ini.value( "Table", "Records");
        case itTABLE:
            return QString ( "%1 x %2").arg( _ini.value( "Table", "Records"), _ini.value( "Table", "Columns"));
    }

    return "";
}

QString ODFItem::stripExtension(const QString& name) const{
    QFileInfo inf(name);
    return inf.baseName();
}

bool ODFItem::isSystemObject(const QString& name) {
    QString temp = QFileInfo(name).baseName().toLower();
    return systemObjectNames.indexOf(name) != -1;

}

QString  ODFItem::cleanName(const QString& name) const{
    if ( name.size() == 0)
        return name;
    QString cleanTxt = name.trimmed();
    if ( cleanTxt.contains(QRegExp("\\\\|/"))) {
        cleanTxt = QFileInfo(cleanTxt).fileName();
    }
    if ( cleanTxt.at(0) == '\'') {
        cleanTxt = cleanTxt.remove("'");
    }
    return cleanTxt.toLower();
}

