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

#ifndef CATALOGSCONTROLLER_H
#define CATALOGSCONTROLLER_H

#include <QQmlContext>
#include "kernel.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "ilwiscontext.h"
#include "resourcemodel.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
namespace Ui {


class ILWISCOREUISHARED_EXPORT CatalogsController : public QObject
{
    Q_OBJECT
public:
    CatalogsController();
    ~CatalogsController();
    void changeCatalogForward( QQmlContext *qmlcontext , const QString& id);
    void changeCatalogBackward( QQmlContext *qmlcontext, const QString &sid);

private:
    Ilwis::Resource getResource(const QString &sid);
    void openCatalog( QQmlContext *qmlcontext, const Ilwis::Resource &resource);
    void getResourceList(const Ilwis::Resource &resource, const QString &query, QList<QObject *> &list);
    void makePathList(const Ilwis::Resource& resource)    ;
    bool _isInitial;
    bool _catalog1IsActive;

     QList<QObject *> pathList;
     QList<QObject *> catalogList1;
     QList<QObject *> catalogList2;
};
}
}

#endif // CATALOGSCONTROLLER_H
