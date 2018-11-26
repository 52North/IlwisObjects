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

#ifndef WORKFLOWIMPL_H
#define WORKFLOWIMPL_H

#include <mutex>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "operation.h"
#include "workflownode.h"
#include "executionnode.h"
#include "workflow.h"
#include "kernel_global.h"

namespace Ilwis {

class Workflow;
typedef IlwisData<Workflow> IWorkflow;


class KERNELSHARED_EXPORT WorkflowImplementation :  public QObject,public OperationImplementation
{
    Q_OBJECT
public:
    WorkflowImplementation();
    WorkflowImplementation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);
    ExecutionNode& executionNode(const SPWorkFlowNode& node, WorkflowIdMapping &mapping);
    ExecutionNode *executionNode(const SPWorkFlowNode &node) ;

    void wait(const Ilwis::SPWorkFlowNode &node);
    void wakeup();
    void sendData(NodeId nodeId, ExecutionContext *ctx, SymbolTable &symTable, const QString &expr);
    bool stopExecution() const;
    void clearCalculatedValues(qint32 nodeid);
    IWorkflow workflow();

signals:
    void sendMessage(const QString& type, const QString& subtype, const QVariantMap& parameters);
public slots:
    void acceptMessage(const QString& type, const QString& subtype, const QVariantMap& parameters);

private:
    IWorkflow _workflow;
    std::map<NodeId, ExecutionNode> _nodes;
    bool _stepMode = false;
    QMutex _syncMutex;
    std::mutex _lock;
    quint32 _runid=iUNDEF;
    bool _initial = true; // the first node may always run(in stepmode) so we ignore waits
    bool _stopExecution = false;

    void clearCalculatedValues();
    void initStepMode(ExecutionContext *ctx);
};


}

#endif // WORKFLOWIMPL_H
