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

#ifndef EXECUTIONNODE_H
#define EXECUTIONNODE_H

namespace Ilwis {
class WorkflowImplementation;
class WorkflowIdMapping;

struct IdTypePair;

typedef std::pair<IdTypePair, IdTypePair> FlowContext;

class ExecutionNode
{
public:
    ExecutionNode();
    ExecutionNode(const SPWorkFlowNode& node, Ilwis::WorkflowIdMapping &mapping);

    bool execute(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping &mapping);
    void clearCalculatedValues();

    QVariant parameterValue(int parmIndex) const;
    void parameterValue(int parmIndex, const QVariant& v) ;
    int parameterCount() const;
    Ilwis::SymbolTable &symbolTableRef();
    static FlowContext contextSwitch(const SPWorkFlowNode &sourceNode, const SPWorkFlowNode &targetNode);

private:
    std::vector<QVariant> _parameterValues;
    SPWorkFlowNode _node;
    SymbolTable _symTable;

    bool executeOperation(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, Ilwis::WorkflowIdMapping &mapping);
    bool executeTest(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping &mapping);
    bool executeJunction(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation* workflowImpl, WorkflowIdMapping &mapping);
    bool executeRange(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, WorkflowIdMapping &mapping);

    void unloadInputs(const std::vector<QString>& objects);
    bool executeRangeJunction(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, WorkflowIdMapping &mapping);
    bool executeRangeTestNode(ExecutionContext *ctx, SymbolTable &symTable, WorkflowImplementation *workflowImpl, WorkflowIdMapping &mapping);

    void clearCalculatedValues(std::vector<SPWorkFlowNode>& operations, WorkflowImplementation *workflowImpl);
    void clearScopedCalcutedValues();

    bool executeLink(ExecutionContext *ctx, SymbolTable &symTable, Ilwis::WorkFlowParameter &paramete, WorkflowImplementation *workflowImpl, WorkflowIdMapping &mapping);
    void setInputJunction(ExecutionContext *ctx, SymbolTable &symTable, const SPWorkFlowNode &operation, WorkflowImplementation *workflowImpl, WorkflowIdMapping &mapping);
};
}

Q_DECLARE_METATYPE(Ilwis::FlowContext)

#endif // EXECUTIONNODE_H
