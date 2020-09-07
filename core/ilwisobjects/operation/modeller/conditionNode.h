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

#ifndef WORKFLOWCONDITION_H
#define WORKFLOWCONDITION_H

#include "ilwis.h"

namespace Ilwis {

class Workflow;
typedef IlwisData<Workflow> IWorkflow;

class WorkFlowNode;
typedef std::shared_ptr<WorkFlowNode> SPWorkFlowNode;

class KERNELSHARED_EXPORT WorkFlowCondition : public WorkFlowNode
{
public:
    enum ConditionParts{cpTEST=0, cpTRUECASE=1, cpFALSECASE=2};
    struct Test{
        SPWorkFlowNode _operation;

        bool isValid() const;
    };

    WorkFlowCondition();

    Ilwis::WorkFlowNode::NodeTypes type() const;

    std::vector<SPWorkFlowNode> subnodes(const QString& reason="") const;
    void addSubNode(const SPWorkFlowNode &node, const QString &reason);
    void removeSubNode(Ilwis::NodeId dNodeId);
    void addTest(SPWorkFlowNode node);
    void setTestValue(int testIndex, int parameterIndex, const QString& value, const IWorkflow &wf);
    QString testValue(int testIndex, int parameterIndex, const IWorkflow &wf) const;
    Test test(int index) const;
    int testCount() const;
    void nodeId(quint64 id);
    bool isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck check) const;

private:
    std::vector<Test> _tests;
    std::vector<SPWorkFlowNode> _operations;
};
typedef std::shared_ptr<WorkFlowCondition> SPCondition;
}

#endif // WORKFLOWCONDITION_H
