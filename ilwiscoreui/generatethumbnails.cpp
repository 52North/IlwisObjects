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


bool GenerateThumbnails::canUse(const std::vector<ResourceModel *>& resources) 
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
