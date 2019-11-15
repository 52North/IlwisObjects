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

#include <functional>
#include <future>
#include <QSqlQuery>
#include <QSqlError>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "representation.h"
#include "table.h"
#include "columndefinition.h"
#include "setrepresentation.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(SetRepresentation)

SetRepresentation::SetRepresentation()
{
}


SetRepresentation::SetRepresentation(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool SetRepresentation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;
	PixelIterator iter(_inputRaster);
	for (auto& v : _outputRaster) {
		v = *iter;
		++iter;
	}

	if (_attributeIndex == iUNDEF)
		_outputRaster->datadefRef().representation(_representation);
	else
		_outputRaster->attributeTable()->columndefinitionRef(_attributeIndex).datadef().representation(_representation);

	QVariant value;
	value.setValue<IRasterCoverage>(_outputRaster);
	logOperation(_outputRaster, _expression, { _inputRaster });
	ctx->setOutput(symTable, value, _outputRaster->name(), itRASTER, _outputRaster->resource());
	return true;

}

Ilwis::OperationImplementation *SetRepresentation::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new SetRepresentation(metaid, expr);
}

Ilwis::OperationImplementation::State SetRepresentation::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	OperationImplementation::prepare(ctx, st);
	QString raster1 = _expression.parm(0).value();

	if (!_inputRaster.prepare(raster1, itRASTER)) {
		ERROR2(ERR_COULD_NOT_LOAD_2, raster1, "");
		return sPREPAREFAILED;
	}
	QString attr = _expression.parm(1).value(); 
	if (attr != PIXELVALUE) {
		if ( !_inputRaster->hasAttributes()){
			kernel()->issues()->log(TR("Attribute isnt valid for this map ") + attr);
			return sPREPAREFAILED;
		}
		ITable tbl = _inputRaster->attributeTable();
		int idx = tbl->columnIndex(attr);
		if (idx == iUNDEF) {
			kernel()->issues()->log(TR("Attribute isnt valid for this map ") + attr);
			return sPREPAREFAILED;
		}
		_attributeIndex = idx;
	}
	int copylist = itDOMAIN | itCOORDSYSTEM | itGEOREF;
	_outputRaster = OperationHelperRaster::initialize(_inputRaster.as<IlwisObject>(), itRASTER, copylist);
	if (!_outputRaster.isValid()) {
		ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
		return sPREPAREFAILED;
	}

	QString rpr = _expression.parm(2).value();
	if (!_representation.prepare(rpr)) {
		ERROR1(ERR_NO_INITIALIZED_1, "Representation");
		return sPREPAREFAILED;
	}
	std::vector<double> indexes(_outputRaster->size().zsize());
	_outputRaster->setDataDefintions(_inputRaster->datadef().domain(), _inputRaster->stackDefinition().indexes());
	if (_inputRaster->hasAttributes()) {
		ITable tbl;
		auto *ptr = _inputRaster->attributeTable()->clone();
		tbl.set(static_cast<Table *>(ptr));
		_outputRaster->setAttributes(tbl, _inputRaster->primaryKey());
	}

	initialize(_outputRaster->size().linearSize());
	return sPREPARED;
}

quint64 SetRepresentation::createMetadata()
{
	OperationResource operation({ "ilwis://operations/setrepresentation" });
	operation.setSyntax("stackminmaxpick(raster1, attribute=Pixel value, representation");
	operation.setDescription(TR("generates a new raster with the same structure as the original but every pixel column \nin the stack will only retain its value if its either the min or the max in his column"));
	operation.setInParameterCount({ 3 });
	operation.addInParameter(0, itRASTER, TR("rastercoverage"), TR("input rastercoverage with any domain domain"));
	operation.addInParameter(1, itSTRING, TR("atribute"), TR("Attribute whose representation needs to be set. For Value maps the default attribute is \"Pixel Value\" "));
	operation.addInParameter(2, itREPRESENTATION, TR("Representation"), TR("The representation used for the selected attribute"));
	operation.setOutParameterCount({ 1 });
	operation.addOutParameter(0, itRASTER, TR("output raster"), TR("output raste"));
	operation.setKeywords("representation,raster");

	mastercatalog()->addItems({ operation });
	return operation.id();
}
