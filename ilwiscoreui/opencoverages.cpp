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
#include "opencoverages.h"

using namespace Ilwis;
using namespace Ui;


OpenCoverages::OpenCoverages() : CatalogOperationEditor("opencoverages", TR("Open Coverages"), QUrl("OpenCoverages.qml"), TR("Opens a set of selected coverages in the same map panel"))
{
}


bool OpenCoverages::canUse(const std::vector<ResourceModel *>& resources) 
{
	for (auto resource : resources) {
		if (hasType(resource->type(), itCOVERAGE))
			return true;
	}
	return false;
}

QVariant OpenCoverages::execute(const QVariantMap &parameters)
{
	try {
		QString side = parameters["side"].toString();
		QString maps1 = parameters["coverages"].toString();
		QStringList maps = maps1.split("|");
		quint32 viewid = iUNDEF;;
		for (const QString& mapid : maps) {
			quint64 id = mapid.toULongLong();
			Resource res = mastercatalog()->id2Resource(id);
			if (!res.isValid() || hasType(res.ilwisType(), itCOVERAGE) == false)
				continue;
			QString expr = QString("showcoverage(%1, %2,\"\")").arg(res.url(true).toString()).arg(side);
			if (viewid != iUNDEF) {
				expr = QString("adddrawer(%1,\"?\",\"itemid=%2\",%3,true,\"\")").arg(viewid).arg(id).arg(IlwisObject::type2Name(res.ilwisType()));
			}

			Ilwis::OperationExpression ex(expr);
			Ilwis::Operation op(ex);
			QVariant v;
			ExecutionContext ctx;
			SymbolTable tbl;
			op->execute(&ctx, tbl);
			if (ctx._results.size() > 0 && viewid == iUNDEF) {
				v = tbl.getValue(ctx._results[0]);
				viewid = v.toInt();
			}
		}
		return viewid;
	}
	catch (const ErrorObject&) {

	}
	return QVariant();
}
