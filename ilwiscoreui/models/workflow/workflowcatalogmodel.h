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

#ifndef WORKFLOWCATALOGMODEL_H
#define WORKFLOWCATALOGMODEL_H

#include <QObject>
#include <QQmlListProperty>

#include "../operationcatalogmodel.h"
#include "workflowmodel.h"
#include "ilwiscoreui_global.h"

namespace Ilwis{
namespace Ui{
class WorkflowModel;

class ILWISCOREUISHARED_EXPORT WorkflowCatalogModel : public CatalogModel
{
    Q_OBJECT
    Q_PROPERTY(QMLWorkflowList workflows READ workflows NOTIFY workflowsChanged)

public:

    explicit WorkflowCatalogModel(QObject *parent = 0);

    QMLWorkflowList workflows();
    void nameFilter(const QString&);

    Q_INVOKABLE quint64 workflowId(quint32 index) const;
    Q_INVOKABLE QString executeworkflow(quint64 workflowid, const QString &parameters);

    Q_INVOKABLE  Ilwis::Ui::WorkflowModel *newWorkflow(const QString &name);
    Q_INVOKABLE  bool deleteWorkflow(quint32 index);
    Q_INVOKABLE  Ilwis::Ui::WorkflowModel *saveWorkflow(quint32 index);

private:
    QList<WorkflowModel *> _currentWorkflows;

signals:
    void error(const QString& err);
    void updateCatalog(const QUrl& url);
    void workflowsChanged();

public slots:

};
}
}

#endif // WORKFLOWCATALOGMODEL_H
