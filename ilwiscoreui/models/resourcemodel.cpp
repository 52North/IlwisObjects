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
#include <QFileInfo>
#include <QUrl>
#include <QDir>
#include "kernel.h"
#include "connectorinterface.h"
#include "catalog/resource.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "ellipsoid.h"
#include "projection.h"
#include "proj4parameters.h"
#include "ilwisobject.h"
#include "mastercatalog.h"
#include "resourcemodel.h"
#include "resourcemanager.h"
#include "oshelper.h"

#define tempHardPath "h:/projects/Ilwis4/projects/client/qml/desktop/mobile/images/"
//#define tempHardPath "d:/projects/ilwis/Ilwis4/projects/client/qml/desktop/mobile/images/"

using namespace Ilwis;
using namespace Ui;

QString ResourceModel::getProperty(const QString &propertyname) const
{
    try{
    if(itemRef().hasProperty(propertyname))
        return itemRef()[propertyname].toString();
    } catch(const ErrorObject&) {}
    return sUNDEF;
}

ResourceModel::ResourceModel()
{

}

ResourceModel::ResourceModel(const Ilwis::Resource& source, QObject *parent) :
    QObject(parent), _imagePath(sUNDEF),_type(itUNKNOWN), _isRoot(false)
{
    resource(source);
}

ResourceModel::ResourceModel(const ResourceModel &model) : QObject(model.parent())
{
    _displayName = model._displayName;

    itemRef() = model.itemRef();
    _imagePath = model._imagePath;
    _type = model._type;
    _isRoot = model._isRoot;
    _selected = model._selected;
    _is3d = model._is3d;
}

ResourceModel &ResourceModel::operator=(const ResourceModel &model)
{
    _displayName = model._displayName;

    itemRef() = model.itemRef();
    _imagePath = model._imagePath;
    _type = model._type;
    _isRoot = model._isRoot;
    _selected = model._selected;
    _is3d = model._is3d;

    return *this;
}

ResourceModel::~ResourceModel()
{}

QString ResourceModel::imagePath() const
{
    return _imagePath;
}

void  ResourceModel::imagePath(const QString& s) {
    _imagePath = s;
    emit imagePathChanged();
}

quint64 ResourceModel::type() const
{
    return _type;
}

QString ResourceModel::typeName() const
{
    return TypeHelper::type2name(_type);
}

QString ResourceModel::name() const
{
    try{
        if ( itemRef().isValid()) {
            return itemRef().name();
        }
    }   catch (const ErrorObject&){}
    return "";
}

QString ResourceModel::size() const
{
    try{
        if ( itemRef().isValid() && itemRef().ilwisType() != itCATALOG){
            quint64 sz = itemRef().size();
            if ( sz != 0)
                return  QString::number(sz);
        }
    } catch (const ErrorObject&){}
    return "";
}

QString ResourceModel::description() const
{
    try {
        if ( itemRef().isValid())
            return itemRef().description();
    } catch (const ErrorObject&){}
    return "";
}

void ResourceModel::setDescription(const QString &desc)
{
    if ( itemRef().isValid()){
        itemRef().setDescription(desc);
        mastercatalog()->changeResource(itemRef().id(), "description",desc);
        emit descriptionChanged();
    }
}

QString ResourceModel::dimensions() const
{
    try{
        if ( itemRef().isValid())
            return itemRef().dimensions();
    } catch (const ErrorObject&){}
    return "";
}

QString ResourceModel::displayName() const
{
    if ( _displayName != "")
        return _displayName;
    return name();
}

void ResourceModel::setDisplayName(const QString &name)
{

    try {
        if ( itemRef().isValid()){
            _displayName = name;
            if ( item().name() == "")
                itemRef().name(name, false);
            mastercatalog()->changeResource(itemRef().id(), "name",name);
            emit displayNameChanged();
        }
    } catch (const ErrorObject&){}
}

QString ResourceModel::url() const
{
    try{
        return itemRef().url().toString();
    } catch (const ErrorObject&){}
    return "";
}

QString ResourceModel::rawUrl() const
{
    try{
        return itemRef().url(true).toString();
    } catch (const ErrorObject&){}
    return "";
}

QString ResourceModel::container() const
{
    try{
        return itemRef().container().toString();
    } catch (const ErrorObject&){}

    return "";
}

QString ResourceModel::containerFile() const
{
    QString containerFile = "";
    if (hasType(item().ilwisType(), itRASTER)) {
        QString path = resource().container().toLocalFile();
        QFileInfo infContainer(path);
        bool containerIsFile = infContainer.isFile();
        if (containerIsFile) {
            path = path.mid(0, resource().container().toLocalFile().lastIndexOf("/"));
            containerFile = resource().container().fileName();
            QDir dir(path + "/.ilwis/thumbs");
            if (!QDir(path + "/.ilwis/thumbs/" + containerFile).exists()) {
                if (!dir.mkdir(containerFile))
                    containerFile = "";
                else
                    containerFile += "/";
            }
            else
                containerFile += "/";
        }
    }

    return containerFile;
}

QString ResourceModel::icon(const Resource& res)
{
    try{
        quint64 tp = res.ilwisType();
        if (hasType(res.extendedType(), itCATALOG)) {
            if (tp == itFEATURE)
                return iconPath(itCOVERAGE | itCATALOG);
            else if (tp == itTABLE || tp == itRASTER)
                return iconPath(tp | itCATALOG);
        }
        if ( hasType(tp, itCOORDSYSTEM))
            return iconPath(itCOORDSYSTEM);
        return iconPath(tp);
    } catch(const ErrorObject& ){

    }
    return "";
}

QString ResourceModel::iconPath() const
{
    try{
        if ( _iconPath != "")
            return _iconPath;

        quint64 tp = itemRef().ilwisType();
        if (hasType(itemRef().extendedType(), itCATALOG)) {
            if (tp == itFEATURE)
                return iconPath(itCOVERAGE | itCATALOG);
            else if (tp == itTABLE || tp == itRASTER)
                return iconPath(tp | itCATALOG);
        }
		if (tp == itCATALOG) {
			if (hasType(itemRef().extendedType(), itRASTER)) {
				return "raster_set2.png";
			}
		}
        if ( hasType(tp, itCOORDSYSTEM))
            return iconPath(itCOORDSYSTEM);
		if (hasType(tp, itDOMAIN) && hasType(itemRef().extendedType(), itDATETIME)) {
			return iconPath(itDATETIME);
		}
        if (displayName() == "..")
            return "folderUp.png";
        return iconPath(tp);
    } catch(const ErrorObject& ){

    }
    return "";
}

void ResourceModel::iconPath(const QString &name)
{
    _iconPath = name;
}

QString ResourceModel::iconPath(IlwisTypes tp)
{
    if ( tp == (itRASTER|itCATALOG))
        return "raster_set.png";
    if ( tp == (itTABLE|itCATALOG))
        return "catalogTable20.png";
    if ( tp == (itCOVERAGE|itCATALOG))
        return "vector_set.png";
   if ( tp == (itOPERATIONMETADATA|itSCRIPT))
        return "operation20.png";
   if (tp & itRASTER)
	   return "object_raster.svg";
   else if (tp == itPOLYGON)
	   return "vector_fill.png";
   else if (tp == itLINE)
	   return "vector_line.png";
   else if (tp == itPOINT)
	   return "vector_point.png";
   else if (hasType(tp, itFEATURE))
	   return "vector.png";
   else if (tp & itTABLE)
	   return "table.png";
   else if (tp == (itCONVENTIONALCOORDSYSTEM | itLOCATION))
	   return "coordinate.png";
   else if (tp == itCONVENTIONALCOORDSYSTEM)
	   return "coordinate.png";
   else if (tp == itBOUNDSONLYCSY)
	   return "coordinate.png";
   else if (tp & itCOORDSYSTEM)
	   return "coordinate.png";
   else if (tp == (itGEOREF | itLOCATION))
	   return "georeference.png";
   else if (tp & itGEOREF)
	   return "georeference.png";
   else if (tp == itCATALOG) {
	   return "folder.png";
   }
   else if (tp == (itTHEMATICITEM | itITEMDOMAIN))
	   return "object_domain_item.svg";
   else if (tp == (itIDENTIFIERITEM | itITEMDOMAIN))
	   return "object_domain_item.svg";
   else if (tp == (itNUMERICITEM | itITEMDOMAIN))
	   return "object_domain_item.svg";
   else if (tp == (itINDEXEDITEM | itITEMDOMAIN))
	   return "object_domain_item.svg";
   else if (tp == (itTIMEITEM | itITEMDOMAIN))
	   return "object_domain_item.svg";
   else if (tp == (itTIME | itDATE))
	   return "object_domain_time.svg";
   else if (tp == (itCOLORDOMAIN))
	   return "object_domain_color.svg";
   else if (tp == (itPALETTECOLOR | itITEMDOMAIN))
	   return "object_domain_item.svg";
   else if (tp == (itNUMERICDOMAIN))
	   return "object_domain_numb.svg";
   else if (tp & itITEMDOMAIN)
	   return "object_domain_item.svg";
   else if (tp & itTEXTDOMAIN)
	   return "textdomain.png";
   else if (tp & itDOMAIN)
	   return "object_domain_general.svg";
   else if (tp == itSCRIPT)
	   return "object_script.svg";
   else if (tp & itREPRESENTATION)
	   return "representation.png";
   else if (hasType(tp, itNUMERICDOMAIN))
	   return "valuedom20.png";
   else if (hasType(tp, itNUMBER))
	   return "numbers20.png";
   else if (hasType(tp, itBOOL))
	   return "bool20.png";
   else if (hasType(tp, itPROJECTION))
	   return "projection.png";
   else if (hasType(tp, itELLIPSOID))
	   return "ellipsoid.png";
   else if (tp & itSTRING)
	   return "text20.png";
   else if (tp & itWORKFLOW)
	   return "object_workflow.svg";
   else if (tp & itOPERATIONMETADATA)
	   return "operation.png";
   else if (tp & itCOORDINATE)
	   return "coord20.png";
   else if (tp & itPIXEL)
	   return "pixel20.png";
   else if (tp & itMODEL)
	   return "model.png";
   else if (tp & itCOMBINATIONMATRIX)
	   return "combomatrix.png";
   else if (tp & itCHART)
	   return "object_chart.svg";
    else
        return "eye.png";
}

quint32 ResourceModel::ref() const
{
    return _ref;
}

void ResourceModel::deref()
{
    --_ref;
}

void ResourceModel::addref()
{
    ++_ref;
}

QString ResourceModel::modelType() const
{
    return "resourcemodel";
}

bool ResourceModel::extendedType() const
{
    if ( _item.hasProperty("extendedtype")){
        return  _item["extendedtype"].toBool();
    }
    return false;
}

void ResourceModel::suicide()
{
    setParent(0);
    // use only when certain
    delete this;
}

Resource &ResourceModel::itemRef()
{
    return _item;
}

const Resource &ResourceModel::itemRef() const
{
    return _item;
}

bool ResourceModel::isRoot() const
{
    return _isRoot;
}

bool ResourceModel::changed() const
{
    return item().hasChanged();
}
QString ResourceModel::id() const
{
    try {
    if ( itemRef().isValid())
        return QString::number(itemRef().id());
    } catch (const ErrorObject&){}
    return sUNDEF;
}

Resource ResourceModel::item() const
{
    return itemRef();
}


QString ResourceModel::domainName() const {
    try{
    QString nme =  propertyName("domain");
    if ( nme != displayName() && nme != "")
        return nme;
    quint64 tp = itemRef().ilwisType();
    if ( hasType(tp, itCOVERAGE))
        return "self";
    } catch(const ErrorObject&) {}
    return "";
}

QString ResourceModel::domainType() const {
    try{
    QString nme = propertyTypeName(itDOMAIN, "domain");
    if ( nme != "")
        return nme;
    quint64 tp = itemRef().ilwisType();
    if ( hasType(tp, itCOVERAGE))
        return "IndexedIdentifier";
    } catch(const ErrorObject&) {}
    return "";
}

QString ResourceModel::proj42DisplayName(const QString& proj4Def) const{
    Proj4Parameters parms(proj4Def);
    QString projName = Projection::projectionCode2Name(parms["proj"]);
    QString ellipse = Ellipsoid::ellipsoidCode2Name(parms["proj"]);
    if ( ellipse == sUNDEF)
        return sUNDEF;
    return projName + "/" + ellipse;
}

QString ResourceModel::coordinateSystemName() const {
    try{
        QString proj = itemRef()["projectionname"].toString();
        if ( proj != sUNDEF)
            return proj;

        QString nme =  propertyName("coordinatesystem");
        if ( nme != displayName() && nme != "" && nme != sUNDEF)
            return nme;
        if ( nme == ""){
            nme = itemRef()["coordinatesystem"].toString();
            if ( nme != sUNDEF){
                int index = nme.toLower().indexOf("code=");
                if ( index == -1){
                    nme = itemRef().code();
                }
                if ((index = nme.toLower().indexOf("code=epsg")) != -1){
                    nme = nme.mid(5);
                }
                else if ( (index = nme.toLower().indexOf("code=proj4")) != -1){
                    nme = proj42DisplayName(nme.mid(5));
                }else {
                    nme = Projection::projectionCode2Name(nme.mid(5));
                }
            }
        }
        return nme != sUNDEF ? nme : "";
    } catch(const ErrorObject&) {}
    return "" ;
}

QString ResourceModel::coordinateSystemType() const {
    QString txt = propertyTypeName(itCOORDSYSTEM, "coordinatesystem");
    return txt.left(txt.size() - QString("CoordinateSystem").size());
}

QString ResourceModel::geoReferenceName() const
{
    QString nme = propertyName("georeference");
    if ( nme != displayName())
        return nme;
    return "";
}

QString ResourceModel::geoReferenceType() const
{
    return propertyTypeName(itGEOREF, "georeference");
}


void ResourceModel::realizeThumbPath(){
    QFileInfo inf(itemRef().url(true).toLocalFile());
    QString path = inf.absolutePath();
    QString thumbDir =  path + "/.ilwis/thumbs";
    QDir dir(thumbDir) ;
    if (!dir.exists()){
        QDir ilwdir(path + "/.ilwis");
        if (!ilwdir.exists())
            if(!QDir(path).mkdir(".ilwis"))
                return;
        if(!QDir(path + "/.ilwis").mkdir("thumbs"))
            return ;
    }
    QFileInfo thumbPath = thumbDir + "/" + itemRef().rawName() + ".png";
    _imagePath = QUrl::fromLocalFile( thumbPath.absoluteFilePath()).toString();
}

void ResourceModel::resource(const Ilwis::Resource& res)
{
    Resource item = res;
    if ( !_item.isValid() && !res.isValid())
        return;

    if ( item.name() == sUNDEF ) {
        QString name  = res.url().toString(QUrl::RemoveScheme | QUrl::RemoveQuery | QUrl::RemovePassword | QUrl::StripTrailingSlash);
        name = name.mid(3);
        item.name(name, false);
    }

    _type = item.ilwisType();
    itemRef() = item;
    if ( hasType(item.ilwisType(), itCOVERAGE)){
        QString dim = itemRef().dimensions();
        _is3d = dim.split(" ").size() == 3;
    }

    if ( item.url().toString() == "file://"){
        _displayName = "root";
        _isRoot = true;
    }
    else if ( item.url().scheme() == "file") {
        QFileInfo inf(itemRef().url().toLocalFile());
        QString path = inf.absolutePath();
        _isRoot = inf.isRoot();
        _displayName = item.name();
        QFileInfo thumbPath = path + "/.ilwis/thumbs/" + containerFile() + item.rawName() + ".png";
        if ( thumbPath.exists()) {
            _imagePath =  QUrl::fromLocalFile(thumbPath.absoluteFilePath()).toString();
        } else {
            if ( item.ilwisType() == itCATALOG) {
                _imagePath = "catalog.png";
            }
            if ( hasType(item.ilwisType(), itRASTER))
                _imagePath = "remote.png";
            else if ( hasType(item.ilwisType(), itFEATURE))
                _imagePath = "polygon.png";
            else if ( hasType(item.ilwisType(), itCOORDSYSTEM))
                _imagePath = "csy.png";
            else if ( hasType(item.ilwisType(), itGEOREF))
                _imagePath = "grf.png";
            else if ( hasType(item.ilwisType(), itTABLE))
                _imagePath = "table.jpg";
            else if ( hasType(item.ilwisType(), itDOMAIN))
                _imagePath = "domainn.png";
            else if ( hasType(item.ilwisType(), itREPRESENTATION))
                _imagePath = "representation20.png";
            else
                _imagePath = "blank.png";
        }
    }else{
        if ( item.hasProperty("longname"))
            _displayName =  item["longname"].toString();
        else
            _displayName = item.name();
    }
}

Ilwis::Resource ResourceModel::resource() const
{
    return itemRef();
}

Ilwis::Resource& ResourceModel::resourceRef()
{
    return itemRef();
}

bool ResourceModel::isSelected() const
{
    return _selected;
}

void ResourceModel::setIsSelected(bool yesno)
{
    _selected = yesno;
    emit isSelectedChanged();
}

QString ResourceModel::propertyName( const QString& property) const{
    if ( itemRef().isValid()) {
        bool ok;
		QVariant value = itemRef()[property];
        quint64 iddomain =  value.toLongLong(&ok);
        if ( ok) {
            return Ilwis::mastercatalog()->id2Resource(iddomain).name();
		}
		else {
			return itemRef()[property].toString();
		}
    }
    return "";

}

QString  ResourceModel::propertyTypeName(quint64 typ, const QString& propertyName) const {
    if ( itemRef().isValid()) {
        if (itemRef().extendedType() & typ) {
            bool ok;
			const QVariant& value = itemRef()[propertyName];
            quint64 idprop =  value.toLongLong(&ok);
            if ( ok) {
                quint64 tp = Ilwis::mastercatalog()->id2Resource(idprop).ilwisType();
                return Ilwis::IlwisObject::type2Name(tp);
			}
			else {
				auto res = mastercatalog()->name2Resource(value.toString());
				if (res.isValid()) {
					return Ilwis::IlwisObject::type2Name(res.ilwisType());
				}

			}
        }
    }
    return "";
}

void ResourceModel::makeParent(QObject *item)
{
    setParent(item);
}

bool ResourceModel::hasExtendedType(const QString &tp) const
{
    try{
        if ( itemRef().isValid()) {
            IlwisTypes typ = Ilwis::IlwisObject::name2Type(tp);
            if ( typ != itUNKNOWN) {
                bool ok = hasType(itemRef().extendedType(), typ);
                return ok;
            }

        }
    } catch (const ErrorObject&){}
    return false;
}

void ResourceModel::setUrl(const QUrl &url, bool asRaw)
{
    itemRef().setUrl(url, asRaw);
}

QString ResourceModel::keywords() const
{
    QString kw = itemRef()["keyword"].toString();
    if ( kw == sUNDEF)
        return "";
    return kw;
}

void ResourceModel::keywords(const QString &pkeys)
{
    QString keys = pkeys;
    QString keywrds = keywords();
    if ( keywrds == ""){
        _item.addProperty("keyword", keys);
        keywrds = keys;
    }else {
        QStringList parts1 = keywrds.split(",");
        QStringList parts2 = keys.split(",");
        for(QString key : parts2){
            if ( !parts1.contains(key.trimmed())){
                keywrds += ","+ key.trimmed()    ;
            }
        }
        _item.addProperty("keyword", keywrds)   ;
    }

    emit keywordsChanged();
}
