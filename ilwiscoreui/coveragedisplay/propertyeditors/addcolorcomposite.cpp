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

#include "coverage.h"
#include "representation.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "operationExpression.h"
#include "coveragelayermodel.h"
#include "mastercatalogmodel.h"
#include "commandhandler.h"
#include "operation.h"
#include "addcolorcomposite.h"

using namespace Ilwis;
using namespace Ui;

AddColorComposite::AddColorComposite()
{

}

AddColorComposite::AddColorComposite(VisualAttribute *p) :
	VisualPropertyEditor(p, "addcolorcomposite", TR("Add Color Composite"), QUrl("AddColorComposite.qml"))
{
}

bool AddColorComposite::canUse(const IIlwisObject& obj, const QString& name) const
{
	return name == VisualAttribute::GLOBAL_ONLY;

}

VisualPropertyEditor *AddColorComposite::create(VisualAttribute *p)
{
	return new AddColorComposite(p);
}

void AddColorComposite::apply(const QString& red, const QString& green, const QString& blue) {
	IRasterCoverage redBand;
	if (!redBand.prepare(red, { "mustexist", true })) {
		kernel()->issues()->log(TR("Red band couldn't be loaded"));
		return;
	}
	IRasterCoverage greenBand;
	if (!greenBand.prepare(green,{ "mustexist", true })) {
		kernel()->issues()->log(TR("Green band couldn't be loaded"));
		return;
	}
	IRasterCoverage blueBand;
	if (!blueBand.prepare(blue, { "mustexist", true })) {
		kernel()->issues()->log(TR("Blue band couldn't be loaded"));
		return;
	}
	
	vpmodel()->layer()->layerManager()->viewid();

	QString cmd = QString("adddrawer(%1, \"\", \"\", coverage, true, \"createtype=colorcomposite,items=%2|%3|%4\")")
		.arg(vpmodel()->layer()->layerManager()->viewid())
		.arg(redBand->id()).arg(greenBand->id()).arg(blueBand->id());
	Ilwis::OperationExpression ex(cmd);
	Ilwis::Operation op(ex);
	QVariant v;
	ExecutionContext ctx;
	SymbolTable tbl;
	op->execute(&ctx, tbl);
}





