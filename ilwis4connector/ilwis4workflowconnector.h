#pragma once

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

#include "ilwis4connector.h"

namespace Ilwis {
	class WorkFlowNode;
	class Workflow;

	typedef std::shared_ptr<WorkFlowNode> SPWorkFlowNode;

	namespace Ilwis4C {

		class Ilwis4WorkflowConnector : public Ilwis4Connector
		{
		public:
			Ilwis4WorkflowConnector(const Ilwis::Resource &resource, bool load, const IOOptions& options = IOOptions());

			bool store(IlwisObject *obj, const IOOptions& options = IOOptions());
			bool loadMetaData(IlwisObject*obj, const IOOptions & options);
			static ConnectorInterface *create(const Ilwis::Resource &resource, bool load, const IOOptions& options = IOOptions());
			bool loadData(IlwisObject *, const IOOptions &options = IOOptions());
			bool storeData(IlwisObject *obj, const IOOptions &options);
			static bool store(IlwisObject *obj, const IOOptions& options, QJsonObject& jroot);
			static bool loadMetaData(IlwisObject* object, const IOOptions&, const QJsonValue& jvalue);

		protected:
			static void storeNode(const SPWorkFlowNode& node, QJsonObject& jnodes, const IOOptions &options) ;
			static void storeNodeLinks(const SPWorkFlowNode& node, QJsonObject& jnode, const IOOptions &options);
			static void loadNodeLinks(const  QJsonObject& jnode, SPWorkFlowNode& node, Workflow *workflow);
			static bool loadNode(QJsonObject jnode, Workflow *wf, SPWorkFlowNode &node);
		};
	}
}
