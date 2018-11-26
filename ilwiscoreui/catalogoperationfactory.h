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

#pragma once

#include "ilwiscoreui_global.h"
#include <map>
#include <vector>
#include <functional>

namespace Ilwis {
	namespace Ui {

		class CatalogOperationEditor;
		class ResourceModel;

		typedef std::function<CatalogOperationEditor *()> CatalogOperationEditorCreate;


		class ILWISCOREUISHARED_EXPORT CatalogOperationFactory : public AbstractFactory
		{
		public:
			CatalogOperationFactory();
			int registerCatalogOperation(const QString& name, CatalogOperationEditorCreate);
			QList<CatalogOperationEditor *>& selectedOperations(const std::vector<ResourceModel *>& resources);
			template<class T = CatalogOperationEditor> T *create(const QString& name)
			{
				auto iter = _creators.find(name);
				if (iter == _creators.end()) {
					return 0;
				}
				return static_cast<T *>((*iter).second());
			}

		private:
			std::map<QString, CatalogOperationEditorCreate> _creators;
			QList<CatalogOperationEditor *> _operations;
		};


	}
}