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
#include <QColor>
#include "ilwisdata.h"
#include "domain.h"
#include "numericdomain.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "createitemrepresentation.h"
#include "representation.h"
#include "itemdomain.h"
#include "domainitem.h"
#include "palettecolorlookup.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateItemRepresentation)

CreateItemRepresentation::CreateItemRepresentation()
{

}

CreateItemRepresentation::CreateItemRepresentation(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool CreateItemRepresentation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	QVariant value;
	PaletteColorLookUp *lookup = new PaletteColorLookUp();
	for (auto& item : _items) {
		lookup->setColor(item.first, item.second);
	}
	IRepresentation rpr;
	rpr.prepare();
	rpr->colors(lookup);
	rpr->domain(_baseDomain);

	value.setValue<IRepresentation>(rpr);
	logOperation(rpr, _expression);
	ctx->setOutput(symTable, value, rpr->name(), itREPRESENTATION, rpr->resource());

	return true;
}

Ilwis::OperationImplementation *CreateItemRepresentation::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new CreateItemRepresentation(metaid, expr);
}

Ilwis::OperationImplementation::State CreateItemRepresentation::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	OperationImplementation::prepare(ctx, st);

	if(!_baseDomain.prepare(_expression.input<QString>(0), { "mustexist", true })){
		kernel()->issues()->log(QString(TR("%1 is not a valid domain")).arg(_expression.input<QString>(0)));
		return sPREPAREFAILED;
	}
	if (_baseDomain->ilwisType() != itITEMDOMAIN) {
		kernel()->issues()->log(QString(TR("%1 is not a item domain")).arg(_expression.input<QString>(0)));
		return sPREPAREFAILED;
	}
	IItemDomain dom = _baseDomain.as< ItemDomain<DomainItem>>();
	auto items = _expression.input<QString>(1);
	items.remove("\"");
	QStringList parts = items.split("|");

	for (int i = 0; i < parts.size(); i+=2) {
		auto item = dom->item(parts[i]);
		QString name = parts[i];
		if (item.isNull()) {
			kernel()->issues()->log(QString(TR("%1 is not part of the domain")).arg(parts[i]));
			return sPREPAREFAILED;
		}
		QString color(parts[i + 1]);
		_items[(quint32)item->raw()] = color;
	}
	return sPREPARED;

}

quint64 CreateItemRepresentation::createMetadata()
{
	OperationResource resource({ "ilwis://operations/createitemrepresentation" });
	resource.setLongName("Create Representation Item Domain");
	resource.setSyntax("createitemrepresentation(basedomain, items)");
	resource.setDescription("Creates a palette based representation for item domains");
	resource.setInParameterCount({ 2 });
	resource.addInParameter(0, itSTRING, TR("Base domain"), TR("The domain which this color scheme is relevant for"));
	resource.addInParameter(1, itSTRING, TR("Item String"), TR("A '|' seprated string of pairs contain an an item name and an item color value"));
	//resource.addOptionalInParameter(5, itDOMAIN, TR("Parent domain"), TR("optional indication of a parent domain"));
	resource.setOutParameterCount({ 1 });
	resource.addOutParameter(0, itREPRESENTATION, TR("output representation"), TR("The newly created representation"));
	resource.setKeywords("domain, create, time,workflow");

	mastercatalog()->addItems({ resource });
	return resource.id();
}
