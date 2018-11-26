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

#ifndef JUNCTION_H
#define JUNCTION_H

#include "kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT JunctionNode : public WorkFlowNode
{
public:
    JunctionNode();
    JunctionNode(quint64 nodeid);
    Ilwis::WorkFlowNode::NodeTypes type() const;
    int inputCount() const;
    void link2trueCase(SPWorkFlowNode trueNode, int parmIndex);
    void link2falseCase(SPWorkFlowNode falseNode, int parmIndex);
    void link2condition(Ilwis::SPWorkFlowNode conditionnode);
    void nodeId(quint64 id);
    bool isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck) const;

private:
};

typedef std::shared_ptr<JunctionNode> SPJunctionNode;
}

#endif // JUNCTION_H
