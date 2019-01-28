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

#ifndef MODELLERMODEL_H
#define MODELLERMODEL_H

#include <QObject>

#include "kernel_global.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "operationmetadata.h"
#include "ilwisobjectmodel.h"
#include "symboltable.h"
#include "location.h"
#include "operationmetadata.h"
#include "workflownode.h"
#include "workflow.h"
#include "applicationmodel.h"
#include "analysispattern.h"
#include "operationExpression.h"
#include "workflowmodel.h"
#include "conceptmodel.h"
#include "analysismodel.h"
#include "applicationmodelui.h"
#include "ilwiscoreui_global.h"
#include "model.h"

namespace Ilwis{
namespace Ui{
class ILWISCOREUISHARED_EXPORT ModelDesigner : public ResourceModel
{
    Q_OBJECT
public:
    explicit ModelDesigner(QObject *parent = 0);
    ModelDesigner(ResourceModel *rmodel, QObject *parent=0);
    Q_PROPERTY(int workflowCount READ workflowCount NOTIFY workflowCountChanged)
    Q_PROPERTY(int analysisCount READ analysisCount NOTIFY analysisCountChanged)
    Q_PROPERTY(int conceptCount READ conceptCount NOTIFY conceptCountChanged)
    Q_PROPERTY(int applicationCount READ applicationCount NOTIFY applicationCountChanged)
    Q_PROPERTY(QStringList workflowNames READ workflowNames CONSTANT)
    Q_PROPERTY(QStringList analysisNames READ analysisNames NOTIFY analysisNamesChanged)
    Q_PROPERTY(WorkflowModel *currentWorkflow READ currentWorkflow WRITE currentWorkflow NOTIFY workflowChanged)
    Q_PROPERTY(QStringList analysisTypes READ analysisTypes NOTIFY analysisTypesChanged)


    qint32 workflowCount() const;
    Q_INVOKABLE Ilwis::Ui::WorkflowModel* workflow(qint32 index) const;
    Q_INVOKABLE Ilwis::Ui::WorkflowModel* workflow(const QString& name) const;
    Q_INVOKABLE Ilwis::Ui::WorkflowModel *addWorkflow(const QString &filter);
    //Q_INVOKABLE bool addWorklfow(const IWorkflow& wf);
    Q_INVOKABLE void removeWorkflow(const QString& name);
    Q_INVOKABLE void removeWorkflow(qint32 index);

    QStringList analysisTypes() const;
    qint32 analysisCount() const;
    Q_INVOKABLE Ilwis::Ui::AnalysisModel *analysisModel(qint32 index) const;
    Q_INVOKABLE bool addAnalysisPattern(Ilwis::Ui::AnalysisModel *amodel);
    Q_INVOKABLE void removeAnalysisPattern(const QString& name);
    Q_INVOKABLE void removeAnalysisPattern(qint32 index);
    Q_INVOKABLE QStringList applicationNames(const QString& analysisName) const; // apps in the current model
    Q_INVOKABLE QStringList applicationsByAnalysis(const QString& analysisType) const; //all apps known based on a type

    qint32 applicationCount() const;
    Q_INVOKABLE Ilwis::Ui::ApplicationModelUI* application(qint32 index) const;
   // SPModelApplication application(const QString& name) const;
    Q_INVOKABLE bool addApplication(Ilwis::Ui::ApplicationModelUI *app);
    Q_INVOKABLE  void removeApplication(const QString& name);
    Q_INVOKABLE void removeApplication(qint32 index);

    Q_INVOKABLE bool store(const QString &location, const QString& name);

   qint32 conceptCount() const;



signals:
    void workflowCountChanged();
    void applicationCountChanged();
    void analysisCountChanged();
    void conceptCountChanged();
    void workflowChanged();
    void analysisTypesChanged();
    void analysisNamesChanged();

public slots:

private:
    std::vector<WorkflowModel*> _workflowmodels;
    std::vector<ApplicationModelUI*> _appmodels;
    std::vector<AnalysisModel*> _analysismodels;
    std::vector<ConceptModel *> _conceptmodels;
    WorkflowModel *_currentWorkflow = 0;
    IModel _model;

    QStringList workflowNames() const;
    QStringList analysisNames() const;
    QStringList conceptNames() const;
    WorkflowModel *currentWorkflow();
    void currentWorkflow(WorkflowModel *wf);
};
}
}

#endif // MODELLERMODEL_H
