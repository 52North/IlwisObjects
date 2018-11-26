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

#ifndef OPERATIONNODE_H
#define OPERATIONNODE_H

#include "kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT OperationNode : public WorkFlowNode
{
public:
    OperationNode();
    OperationNode(quint64 operationid);
    OperationNode(const QString &name, const QString &description="", quint64 id=i64UNDEF);

    IOperationMetaData operation() const;
    void operation(const QString& provider, const QString& syntax, bool isWorkflow);
    Ilwis::WorkFlowNode::NodeTypes type() const;
    bool isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck vc) const ;
    QString label() const;
    bool isWorkflow() const;

private:
    IOperationMetaData _operation;
    QString _syntax;
    QString _provider;
    bool _isWorkflow = false;
};
typedef std::shared_ptr<OperationNode> SPOperationNode;
}



#endif // OPERATIONNODE_H
