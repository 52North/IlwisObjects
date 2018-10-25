#include "kernel.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "chartmodel.h"
#include "catalogoperationeditor.h"
#include "catalogoperationfactory.h"

using namespace Ilwis;
using namespace Ui;


CatalogOperationFactory::CatalogOperationFactory() : AbstractFactory("CatalogOperationFactory", "ilwis", "Generates operation objects for UI accessible chart operations")
{
}

int CatalogOperationFactory::registerCatalogOperation(const QString &name, CatalogOperationEditorCreate createFunc)
{
	_creators[name] = createFunc;

	return 42;
}

QList<CatalogOperationEditor *>& CatalogOperationFactory::selectedOperations(const std::vector<ResourceModel *>& resources)
{
	_operations.clear();
	for (auto creator : _creators) {
		auto *operation = creator.second();
		if (operation) {
			if (operation->canUse(resources)) {
				_operations.append(operation);
			}
			else
				delete operation;
		}
	}
	return _operations;
}


