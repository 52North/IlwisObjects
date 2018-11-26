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

#ifndef GEODRAWER_H
#define GEODRAWER_H

#include <QtQuick/QQuickFramebufferObject>
#include "kernel.h"
#include <deque>
#include "visualizationmanager.h"

struct DrawerIdTag {
    DrawerIdTag(const Resource& resource) : _resource(resource){}
    DrawerIdTag(const QString& name, bool ascode) : _drawerName(name),_asCode(ascode){}
    Resource _resource;
    quint64 _drawerid = iUNDEF;
    QString _drawerName;
    QString _drawerCode;
    bool _asCode;
};

class GeoRenderer;

class GeoDrawer : public QQuickFramebufferObject
{

    Q_OBJECT
public:
    friend class GeoRenderer;

    Q_INVOKABLE void addDataSource(const QString& url, const QString& typeName);
    //Q_INVOKABLE void addDrawer(const QString& drawercode, const QVariantMap& properties);
    Q_INVOKABLE void setAttribute(const QString &drawercode, const QVariantMap& value);
    Q_INVOKABLE void removeDrawer(const QString& namecode, bool ascode);




    Renderer *createRenderer() const;

private:
    std::vector<DrawerIdTag> _datasources;
    std::deque<DrawerIdTag> _removedDrawers;
    std::deque<std::pair<QString, QVariantMap>> _attributeQueue;
};

#endif // GEODRAWER_H

