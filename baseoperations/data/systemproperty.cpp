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
#include "raster.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "coordinatesystem.h"
#include "ellipsoid.h"
#include "projection.h"
#include "proj4parameters.h"
#include "conventionalcoordinatesystem.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "systemproperty.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(SystemProperty)

SystemProperty::SystemProperty()
{
}

SystemProperty::SystemProperty(quint64 metaid, const Ilwis::OperationExpression &expr) :
	OperationImplementation(metaid, expr)
{
}

bool SystemProperty::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;
	QString value = "?";
	if (_systemProperty == "workingcatalog")
		value = context()->workingCatalog()->resource().url().toString();

	logOperation(_expression);
	ctx->setOutput(symTable, value, sUNDEF, itSTRING, Resource());
	return true;
}

Ilwis::OperationImplementation *SystemProperty::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new SystemProperty(metaid, expr);
}

Ilwis::OperationImplementation::State SystemProperty::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	OperationImplementation::prepare(ctx, st);

	std::vector<QString> names{ "workingcatalog" };
	_systemProperty = _expression.parm(0).value().toLower().trimmed();
	if (std::find(names.begin(), names.end(), _systemProperty) == names.end()) {
		kernel()->issues()->log(TR("SystemProperty is not in the list of recognized properties:" + _systemProperty));
		return sPREPAREFAILED;
	}


	return sPREPARED;
}

quint64 SystemProperty::createMetadata()
{

	OperationResource operation({ "ilwis://operations/property" });
	operation.setSyntax("property(systemproperty=workingcatalog)");
	operation.setDescription(TR("returns value of the indicated system property)"));
	operation.setInParameterCount({ 1 });
	operation.addInParameter(0, itSTRING, TR("Requested system property value"), TR("Any SystemProperty than can be translated to a string or number"));
	operation.setOutParameterCount({ 1 });
	operation.addOutParameter(0, itSTRING, TR("value"), TR("value of the indicated system property"));
	operation.setKeywords("workflow");

	mastercatalog()->addItems({ operation });
	return operation.id();

}

