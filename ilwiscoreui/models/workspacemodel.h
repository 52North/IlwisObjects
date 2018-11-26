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

#ifndef WORKSPACEMODEL_H
#define WORKSPACEMODEL_H

#include "catalogmodel.h"
#include "resourcemodel.h"
#include "operationmodel.h"

namespace Ilwis {
namespace Ui{


class ILWISCOREUISHARED_EXPORT WorkSpaceModel : public CatalogModel
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<Ilwis::Ui::ResourceModel> data READ data NOTIFY dataChanged)
    Q_PROPERTY(QQmlListProperty<Ilwis::Ui::OperationModel> operations READ operations NOTIFY operationsChanged)
    Q_PROPERTY(bool isDefault READ isDefault CONSTANT)

public:
    WorkSpaceModel(const QString& name="", QObject *parent = 0);
    ~WorkSpaceModel();

    Q_INVOKABLE void addItems(const QString &ids);
    Q_INVOKABLE void removeItems(const QString &ids);

    bool isDefault() const;

    QString modelType() const;
signals:
    void dataChanged();
    void operationsChanged();
protected:
    void gatherItems();
private:
    QQmlListProperty<OperationModel> operations();
    QQmlListProperty<ResourceModel> data();

    QList<OperationModel *> _operations;
    QList<ResourceModel *> _data;



};
}
}
#endif // WORKSPACEMODEL_H
