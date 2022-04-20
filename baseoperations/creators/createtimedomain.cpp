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
#include "domain.h"
#include "numericdomain.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "createtimedomain.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateTimeDomain)

CreateTimeDomain::CreateTimeDomain()
{

}

CreateTimeDomain::CreateTimeDomain(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool CreateTimeDomain::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;
	IIlwisObject obj;
	Resource res = Resource(itNUMERICDOMAIN);
	res.prepare();
	obj.prepare(res);
	obj->setDescription(_domaindesc);
	obj->resourceRef().setExtendedType(itDATETIME);
	ITimeDomain timdom = obj.as<NumericDomain>();

	TimeInterval numrange(_vmin, _vmax, _vres);
	timdom->range(numrange.clone());
	if (_parent.isValid())
		timdom->setParent(_parent);

	QVariant value;
	value.setValue<IDomain>(timdom);
	logOperation(timdom, _expression);
	ctx->setOutput(symTable, value, timdom->name(), itDOMAIN, timdom->resource());

	return true;
}

Ilwis::OperationImplementation *CreateTimeDomain::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new CreateTimeDomain(metaid, expr);
}

Ilwis::OperationImplementation::State CreateTimeDomain::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	OperationImplementation::prepare(ctx, st);
	_vmin = _expression.input<QString>(0);
	_vmax = _expression.input<QString>(1);
	_vres = _expression.input<QString>(2);

	if (!_vmin.isValid() || !_vmax.isValid() || !_vres.isValid()) {
		kernel()->issues()->log(TR("Iso string for time are incorrect"));
		return sPREPAREFAILED;
	}

	_domaindesc = _expression.input<QString>(3);
	return sPREPARED;
}

quint64 CreateTimeDomain::createMetadata()
{
	OperationResource resource({ "ilwis://operations/createtimeddomain" });
	resource.setLongName("Create Time Domain");
	resource.setSyntax("createtimedomain(min,max,resolution, description)");
	resource.setInParameterCount({ 4 });
	resource.addInParameter(0, itSTRING, TR("Minimum value"), TR("Lowest time in the time domain, iso compatible string"));
	resource.addInParameter(1, itSTRING, TR("Maximum value"), TR("Highest time in the time range, iso compatible string"));
	resource.addInParameter(2, itSTRING, TR("Resolution"), TR("time interval between the numbers in the range, iso compatible string"));
	resource.addInParameter(3, itSTRING, TR("Description"), TR("Extra information about the new domain"));
	//resource.addOptionalInParameter(5, itDOMAIN, TR("Parent domain"), TR("optional indication of a parent domain"));
	resource.setOutParameterCount({ 1 });
	resource.addOutParameter(0, itDOMAIN, TR("output domain"), TR("The newly created domain"));
	resource.setKeywords("domain, create, time,workflow");

	mastercatalog()->addItems({ resource });
	return resource.id();
}
