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