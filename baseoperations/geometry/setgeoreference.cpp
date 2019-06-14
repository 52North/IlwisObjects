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

#include "raster.h"
#include "table.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "mastercatalog.h"
#include "setgeoreference.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(SetGeoreference)

SetGeoreference::SetGeoreference()
{
}

SetGeoreference::SetGeoreference(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool SetGeoreference::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;
	_outputRaster->georeference(_inputGrf);
	PixelIterator iter(_raster);
	PixelIterator iterOut(_outputRaster);
	auto end = iter.end();
	while (iter != end) {
		double val = *iter;
		*iterOut = val;
		updateTranquilizer(iterOut.linearPosition(), 1000);
		++iter;
		++iterOut;
	};


	QVariant value;
	value.setValue<IRasterCoverage>(_outputRaster);
	_outputRaster->addDescription(_expression.toString());
	ctx->setOutput(symTable, value, _outputRaster->name(), itRASTER, _outputRaster->resource());
	return true;
}

Ilwis::OperationImplementation *SetGeoreference::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new SetGeoreference(metaid, expr);
}

Ilwis::OperationImplementation::State SetGeoreference::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	OperationImplementation::prepare(ctx, st);
	QString objectName = _expression.parm(0).value();
	if (!_raster.prepare(objectName)) {
		ERROR2(ERR_COULD_NOT_LOAD_2, "raster", objectName);
		return sPREPAREFAILED;
	}
	objectName = _expression.parm(1).value();
	if (!_inputGrf.prepare(objectName)) {
		if (!_raster.prepare(objectName)) {
			ERROR2(ERR_COULD_NOT_LOAD_2, "georeference", objectName);
			return sPREPAREFAILED;
		}
	}
	if (_inputGrf->size().xsize() != _raster->size().xsize() || 
		_inputGrf->size().ysize() != _raster->size().ysize()) {

		kernel()->issues()->log(TR("GeoReference is not of the correct size (x,y)"));
		return sPREPAREFAILED;
	}

	OperationHelperRaster helper;
	helper.initialize(_raster, _outputRaster, itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF | itDOMAIN);
	auto indexes = _raster->stackDefinition().indexes();
	_outputRaster->setDataDefintions(_raster->datadef().domain(), indexes);

	initialize(_outputRaster->size().linearSize());
	return sPREPARED;
}

quint64 SetGeoreference::createMetadata()
{

	OperationResource operation({ "ilwis://operations/setgeoreference" });
	operation.setSyntax("setgeoreference(inputraster,inputgeoreference");
	operation.setLongName("Set value range raster");
	operation.setDescription(TR("creates a copy of the inputmap with the new georeference"));
	operation.setInParameterCount({ 2 });
	operation.addInParameter(0, itRASTER, TR("input raster"), TR("Raster which needs a new georeference "));
	operation.addInParameter(1, itGEOREF, TR("input georeference"), TR("New georeference, it must have the same size (x,y) as the input map else use resample"));
	operation.setOutParameterCount({ 1 });
	operation.addOutParameter(0, itRASTER, TR("raster coverage"));
	operation.setKeywords("raster,geometry");

	mastercatalog()->addItems({ operation });
	return operation.id();
}
