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

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QLibrary>
#include <QLibraryInfo>
#include "kernel.h"
#include "ilwiscontext.h"
#include "supportlibraryloader.h"
#include "oshelper.h"

using namespace Ilwis;

SupportLibraryLoader::SupportLibraryLoader(const QFileInfo &configPath) : _configLocation(configPath)
{
    if ( !configPath.exists())
        return;

    QString operatingSystem = OSHelper::operatingSystem();
    QString path = configPath.absoluteFilePath();
    QFile file;
    file.setFileName(path);
    if (file.open(QIODevice::ReadOnly)) {
        QString settings = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(settings.toUtf8());
        if ( !doc.isNull()){
            QJsonObject obj = doc.object();
            QJsonValue osses = obj.value("OperatingSystem");
            if ( osses.isArray()){
                QJsonArray arrOsses = osses.toArray();
                for(auto jvalue : arrOsses) {
                    QJsonObject os = jvalue.toObject();
                    if ( os["name"].toString() == operatingSystem){
                        QJsonArray libraries = os["libraries"].toArray();
                        for(auto oslibsvalue : libraries){
                            QJsonObject osprops = oslibsvalue.toObject();
							if (osprops.contains("dlibrary") && QLibraryInfo::isDebugBuild()) {
								_order[osprops["order"].toDouble()] = osprops["dlibrary"].toString();
							}else
								_order[osprops["order"].toDouble()] = osprops["library"].toString();
                        }
                        return;

                    }
                }
            }
        }
    }
}
QString SupportLibraryLoader::order2name(quint32 order) const{
    auto iter = _order.find(order);
    if ( iter != _order.end())
        return iter->second;
    return sUNDEF;
}

bool SupportLibraryLoader::isValid() const
{
    return _order.size() > 0;
}

void SupportLibraryLoader::loadLibraries() const{
    std::map<quint32, QString> order;
    QLibrary lib;
    bool ok = order.size() > 0;
    for(auto name : _order){
        QFileInfo file;
        if (QFileInfo(name.second).exists()) {
            file = QFileInfo(name.second);
        } else {
			QString p = _configLocation.absolutePath() + "/../" + name.second;
            file = QFileInfo(p);
        }
        QString path = file.canonicalFilePath();
        lib.setFileName(path);
        ok = lib.load();
        if ( !ok){
            kernel()->issues()->log(TR("Could not load library ") + path + ",error :" + lib.errorString());
        }
    }
}
