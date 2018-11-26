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

#ifndef CATALOGVIEWMANAGER_H
#define CATALOGVIEWMANAGER_H

#include <QObject>
#include <QTimer>
#include <set>
#include <memory>
#include <mutex>
#include "kernel_global.h"


typedef std::set<QUrl> UrlSet;

namespace Ilwis {
namespace Ui{

class CatalogModel;

class CatalogViewManager : public QObject
{
    Q_OBJECT


public:
    friend CatalogViewManager* catalogViewManager(QObject *parent);

    void registerCatalogModel(CatalogModel *model);
    void unRegisterCatalogModel(CatalogModel *model);

signals:

public slots:
    void updateCatalogViews(const UrlSet& containers);
    void doUpdate();

private:
    explicit CatalogViewManager(QObject *parent = 0);
    std::map<CatalogModel *, bool> _catalogViews;

    static CatalogViewManager *_catalogViewManager;
    static QTimer *_timer;
    std::mutex _lock;
};

CatalogViewManager* catalogViewManager(QObject *parent=0);
}
}




#endif // CATALOGVIEWMANAGER_H
