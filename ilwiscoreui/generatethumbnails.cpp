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
#include "generatethumbnails.h"

using namespace Ilwis;
using namespace Ui;


GenerateThumbnails::GenerateThumbnails() : CatalogOperationEditor("generatethumbnails", TR("Generate Thumbnails"), QUrl("GenerateThumbs.qml"), TR("Generates a thumbnails with default representation for all selected coverages"))
{
}


bool GenerateThumbnails::canUse(const std::vector<ResourceModel *>& resources) const
{
	for (auto resource : resources) {
		if (hasType(resource->type(), itCOVERAGE))
			return true;
	}
	return false;
}

QVariant GenerateThumbnails::execute(const QVariantMap &parameters)
{
	return QVariant();
}
