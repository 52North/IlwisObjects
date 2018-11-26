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
#include <QDir>
#include <QDesktopServices>
#include "ilwisconfiguration.h"
#include "ilwiscontext.h"
#include "dataformat.h"
#include "preferencesmodel.h"

using namespace Ilwis;

QString PreferencesModel::preferedDataFormat(const QString &type)
{
    IlwisTypes ilwtype;
    ilwtype = IlwisObject::name2Type(type);
    if (ilwtype == itUNKNOWN)
        return QString();
    if ( hasType(ilwtype,itFEATURE )){
        ilwtype = itFEATURE;
    }
    QString expr = QString("(datatype & %1)!=0 and (readwrite='rc' or readwrite='rcu')").arg(ilwtype);
    QString formatCode = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/default-conversion-format-" + type,QString(sUNDEF));
    if ( formatCode == sUNDEF){
           switch (ilwtype){
        case itRASTER:
            formatCode = "rastercoverage";break;
        case itPOINT:
        case itLINE:
        case itPOLYGON:
        case itFEATURE:
            formatCode = "featurecoverage";break;
        case itTABLE:
            formatCode =  "table";break;
        case itDOMAIN:
            formatCode = "domain";break;
        case itCOORDSYSTEM:
            formatCode =  "coordinatesystem";break;
        case itGEOREF:
            formatCode =  "georeference"; break;
        default:
            formatCode = sUNDEF;
        }
        expr += " and connector='stream'";
    }

    std::multimap<QString, Ilwis::DataFormat>  formats = Ilwis::DataFormat::getSelectedBy(Ilwis::DataFormat::fpNAME, expr);
    for(auto &format : formats)    {
        if ( formatCode == format.second.property(Ilwis::DataFormat::fpCODE).toString())
            return format.second.property(Ilwis::DataFormat::fpNAME).toString();
    }
    return sUNDEF;
}

void PreferencesModel::setPreferedDataFormat(const QString &type, const QString &name)
{
    if ( name != "" && name != sUNDEF)
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/default-conversion-format-" + type,name);
}

PreferencesModel::PreferencesModel(QObject *parent) : QObject(parent)
{

}

QString PreferencesModel::cacheLocation() const
{
     QString location = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/cache-location",QString(sUNDEF));
     if ( location == sUNDEF || !QFileInfo(QUrl(location).toLocalFile()).exists()) {
        return QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)).toString();
     }
     return location;
}

void PreferencesModel::setCacheLocation(const QString &loc)
{
    QUrl url(loc);
    QFileInfo inf(url.toLocalFile());
    if ( inf.exists()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/cache-location",loc);
    }

}

QString PreferencesModel::internalCatalog() const
{
    QString location = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/internalcatalog-location",QString(sUNDEF));
    if ( location == sUNDEF || !QFileInfo(QUrl(location).toLocalFile()).exists()) {
       return QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/internalcatalog").toString();
    }
    return location;
}

void PreferencesModel::setInternalCatalog(const QString &loc)
{
    QUrl url(loc);
    QFileInfo inf(url.toLocalFile());
    if ( inf.exists()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/internalcatalog-location",loc);
    }
}

double PreferencesModel::uiScale() const
{
    double sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/displayscale",rUNDEF);
    if ( sc == rUNDEF || sc <= 0.1 || sc >= 5)
        return 1.0;
    return sc;

}

void PreferencesModel::uiScale(double sc)
{
    if ( sc > 0.1 && sc <= 5){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/displayscale",QString::number(sc));
        emit uiScaleChanged();
    }

}
