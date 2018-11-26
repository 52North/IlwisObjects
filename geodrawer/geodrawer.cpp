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

#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include "visualizationmanager.h"
#include "coverage.h"
#include "geodrawer.h"
#include "georenderer.h"

QQuickFramebufferObject::Renderer *GeoDrawer::createRenderer() const
{
    return new GeoRenderer();
}

void GeoDrawer::setAttribute(const QString &drawercode, const QVariantMap &values)
{
    _attributeQueue.push_back(std::pair<QString, QVariantMap>(drawercode, values));

}

void GeoDrawer::removeDrawer(const QString &namecode, bool ascode)
{
    _removedDrawers.push_back({namecode, ascode});
    for(auto iter = _datasources.begin(); iter != _datasources.end(); ++iter){
        if ( ascode){
            if ( (*iter)._drawerCode == namecode){
                _datasources.erase(iter);
                break;
            }
        }else if ((*iter)._drawerName == namecode) {
            _datasources.erase(iter);
        }
    }
}

void GeoDrawer::addDataSource(const QString &url, const QString &typeName)
{
    try {
        if ( url == "" || typeName == "")
            return;


        IlwisTypes tp = Ilwis::IlwisObject::name2Type(typeName);
        if ( tp == itUNKNOWN)
            return;


        quint64 id = mastercatalog()->url2id(QUrl(url),tp);
        if ( id == i64UNDEF){
            ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, TR("Visualization"), url);
            return ;
        }
        Resource resource = mastercatalog()->id2Resource(id);
        if (! resource.isValid()){
            ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, TR("Visualization"), url);
            return ;
        }

        _datasources.push_back( DrawerIdTag(resource));

    } catch ( const ErrorObject& err){

    } catch ( const std::exception& ex){
        kernel()->issues()->log(ex.what());
    }
}


