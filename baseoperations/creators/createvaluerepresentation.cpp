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
#include "createvaluerepresentation.h"
#include "representation.h"
#include "itemrange.h"
#include "colorrange.h"
#include "continuouscolorlookup.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateValueRepresentation)

CreateValueRepresentation::CreateValueRepresentation()
{

}

CreateValueRepresentation::CreateValueRepresentation(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool CreateValueRepresentation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	QVariant value;
	ContinuousColorLookup *lookup = new ContinuousColorLookup();
	for (auto& item : _parts) {
		lookup->addGroup(NumericRange(item._min, item._max), ContinuousColorRange(item._cmin, item._cmax));
	}
	lookup->steps(_steps);
	lookup->relative(_relative);
	IRepresentation rpr;
	rpr.prepare();
	rpr->domain(_baseDomain);
	rpr->colors(lookup);
	bool dummy;
	rpr->colors()->definition(_baseDomain, dummy);


	value.setValue<IRepresentation>(rpr);
	logOperation(rpr, _expression);
	ctx->setOutput(symTable, value, rpr->name(), itREPRESENTATION, rpr->resource());

	return true;
}

Ilwis::OperationImplementation *CreateValueRepresentation::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new CreateValueRepresentation(metaid, expr);
}

Ilwis::OperationImplementation::State CreateValueRepresentation::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	OperationImplementation::prepare(ctx, st);

	if (!_baseDomain.prepare(_expression.input<QString>(0), { "mustexist", true })) {
		kernel()->issues()->log(QString(TR("%1 is not a valid domain")).arg(_expression.input<QString>(0)));
		return sPREPAREFAILED;
	}
	if (_baseDomain->ilwisType() != itNUMERICDOMAIN) {
		kernel()->issues()->log(QString(TR("%1 is not a item domain")).arg(_expression.input<QString>(0)));
		return sPREPAREFAILED;
	}
	INumericDomain dom = _baseDomain.as< NumericDomain>();
	auto items = _expression.input<QString>(1);
	items.remove("\"");
	QStringList parts = items.split(";");
	bool ok;
	for (auto part : parts) {
		QStringList values = part.split("|");
		if (values.size() != 3) {
			kernel()->issues()->log(QString(TR("Invalid number of values for representation ")));
			return sPREPAREFAILED;
		}
		auto numbers = values[0].split(":");
		double nmin = numbers[0].toDouble(&ok);
		if (!ok) {
			kernel()->issues()->log(QString(TR("Invalid value for representation ")) + parts[0]);
			return sPREPAREFAILED;
		}
		double nmax = numbers[1].toDouble(&ok);
		if (!ok) {
			kernel()->issues()->log(QString(TR("Invalid value for representation ")) + parts[1]);
			return sPREPAREFAILED;
		}
		QColor clrMin(values[1]);
		if (!clrMin.isValid()) {
			kernel()->issues()->log(QString(TR("Invalid color for representation ")) + parts[2]);
			return sPREPAREFAILED;
		}
		QColor clrMax(values[2]);
		if (!clrMax.isValid()) {
			kernel()->issues()->log(QString(TR("Invalid color for representation ")) + parts[3]);
			return sPREPAREFAILED;
		}
		Info inf{ nmin, nmax, clrMin, clrMax};
		_parts.push_back(inf);
	}
	QString b = _expression.parm(2).value().toLower();
	_relative = b == "true" || b == "yes" || b == "1";
	_steps = _expression.input<int>(3);
	if (_steps < 1)
		_steps = 20;

	return sPREPARED;

}

quint64 CreateValueRepresentation::createMetadata()
{
	OperationResource resource({ "ilwis://operations/createvaluerepresentation" });
	resource.setLongName("Create Representation Value Domain");
	resource.setSyntax("createvaluerepresentation(basedomain, items, relative, steps)");
	resource.setDescription("Creates a palette based representation for item domains");
	resource.setInParameterCount({ 4 });
	resource.addInParameter(0, itSTRING, TR("Base domain"), TR("The domain which this color scheme is relevant for"));
	resource.addInParameter(1, itSTRING, TR("Item String"), TR("A '|' seprated string of quadruples contain an an min/max and an item color min/maxvalue"));
	resource.addInParameter(2, itBOOL, TR("Relative"), TR("The values in the representation are relative towards the actual values they are used on."));
	resource.addInParameter(3, itINTEGER, TR("Steps"), TR("The number of steps each part of the representartion is seperated in"));
	resource.setOutParameterCount({ 1 });
	resource.addOutParameter(0, itREPRESENTATION, TR("output representation"), TR("The newly created representation"));
	resource.setKeywords("domain, create, representation");

	mastercatalog()->addItems({ resource });
	return resource.id();
}
