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

#ifndef WORKFLOWSERIALIZERV1_H
#define WORKFLOWSERIALIZERV1_H

namespace Ilwis {
class WorkFlowNode;
class Workflow;

typedef std::shared_ptr<WorkFlowNode> SPWorkFlowNode;

namespace Stream {

class WorkflowSerializerV1 : public OperationMetadataSerializerV1
{
public:
    WorkflowSerializerV1(QDataStream& stream, const QString &version);

    bool store(IlwisObject *obj, const IOOptions& options = IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & );
    static VersionedSerializer *create(QDataStream &stream, const QString &version);
private:
    bool storeNode(const Ilwis::SPWorkFlowNode &node, const IOOptions &options=IOOptions());
    bool loadNode(Ilwis::SPWorkFlowNode &node, Workflow *workflow, const IOOptions &options=IOOptions());
    void storeNodeLinks(const Ilwis::SPWorkFlowNode &node);
    void loadNodeLinks(SPWorkFlowNode &node, Workflow *workflow);
};
}
}

#endif // WORKFLOWSERIALIZERV1_H
