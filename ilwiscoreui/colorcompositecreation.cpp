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
#include "raster.h"
#include "commandhandler.h"
#include "operation.h"
#include "mastercatalog.h"
#include "operationhelper.h"
#include "uicontextmodel.h"
#include "modelregistry.h"
#include "layermanager.h"
#include "colorcompositecreation.h"

using namespace Ilwis;
using namespace Ui;


ColorCompositeCreation::ColorCompositeCreation() : CatalogOperationEditor("ColorCompositeCreation", TR("Create ColorComposite"), QUrl("ColorComposite.qml"), TR("Creates a false color image (RGB) from a combination of numerical raster bands"))
{
}


bool ColorCompositeCreation::canUse(const std::vector<ResourceModel *>& resources) 
{
	if (resources.size() != 3)
		return false;
	QUrl container;
	IGeoReference commonGrf;
	bool ok = false;
	_bands = std::vector<ResourceModel *>();

	for (auto resource : resources) {
		if (!hasType(resource->type(), itRASTER))
			return false;
		if (!container.isValid())
			container = resource->item().container();
		if (resource->item().container() != container)
			return false;
		IRasterCoverage raster;
		if (!raster.prepare(resource->item()))
			return false;
		if (raster->datadef().domain()->ilwisType() != itNUMERICDOMAIN)
			return false;
		if (!commonGrf.isValid())
			commonGrf = raster->georeference();
		if (!raster->georeference()->isCompatible(commonGrf))
			return false;
		ok = true;
	}
	if (ok) {
		for (auto resource : resources) {
			_bands.push_back(resource);
		}
		emit bandsChanged();
	}

	return ok;
}

QString ColorCompositeCreation::red() const {
	if (_bands.size() == 3) {
		return _bands[0]->item().url().toString();
	}
	return "";
}

QString ColorCompositeCreation::green() const {
	if (_bands.size() == 3) {
		return _bands[1]->item().url().toString();
	}
	return "";
}

QString ColorCompositeCreation::blue() const {
	if (_bands.size() == 3) {
		return _bands[2]->item().url().toString();
	}
	return "";
}


QVariant ColorCompositeCreation::execute(const QVariantMap &parameters)
{

	QString side = parameters["side"].toString();
	QString maps1 = parameters["rasters"].toString();
	QStringList maps = maps1.split("|");
	quint32 viewid = iUNDEF;
	QString url;
	QString ids;
	for (const QString& mapid : maps) {
		quint64 id = mapid.toULongLong();
		Resource res = mastercatalog()->id2Resource(id);
		if (ids != "") {
			ids += '|';
		}
		if (!res.isValid() || hasType(res.ilwisType(), itRASTER) == false)
			continue;
		ids += QString::number(id);
	}
	IRasterCoverage baseMap;
	if (!baseMap.prepare(maps[0].toULongLong(), { "mustexist", true }))
		return QVariant();
	IRasterCoverage colorcomposite;
	colorcomposite.prepare();
	colorcomposite->resourceRef().name("colorcomposite", false, false);
	colorcomposite->coordinateSystem(baseMap->coordinateSystem());
	colorcomposite->georeference(baseMap->georeference());
	std::vector<double> indexes = { 0 };
	colorcomposite->setDataDefintions(baseMap->datadef().domain(), indexes);
	colorcomposite->store();
	mastercatalog()->addItems({ colorcomposite->resource()}); 

	QString expr = QString("showcoverage(%1, %2,\"createtype=colorcomposite,items=%3\")").arg(colorcomposite->resource().url().toString()).arg(side).arg(ids);
	Ilwis::OperationExpression ex(expr);
	Ilwis::Operation op(ex);
	QVariant v;
	ExecutionContext ctx;
	SymbolTable tbl;
	if (op->execute(&ctx, tbl)) {
		if (ctx._results.size() > 0 && viewid == iUNDEF) {
			v = tbl.getValue(ctx._results[0]);
			viewid = v.toInt();
			return viewid;
		}
	}
	return QVariant();
}


