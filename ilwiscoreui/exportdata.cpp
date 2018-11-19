#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "domain.h"
#include "table.h"
#include "commandhandler.h"
#include "operation.h"
#include "mastercatalog.h"
#include "operationhelper.h"
#include "uicontextmodel.h"
#include "modelregistry.h"
#include "ExportData.h"

using namespace Ilwis;
using namespace Ui;


ExportData::ExportData() : CatalogOperationEditor("exportdata", TR("Export objects"), QUrl("ExportData.qml"), TR("Converts (when possible) selected objects to a different data format"))
{
}


bool ExportData::canUse(const std::vector<ResourceModel *>& resources) 
{
	for (auto resource : resources) {
		if (hasType(resource->type(), itCOVERAGE | itTABLE))
			return true;
	}
	return false;
}

QVariant ExportData::execute(const QVariantMap &parameters)
{
	return QVariant();
}


