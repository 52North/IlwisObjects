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
#include "lookupindex.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(LookUpIndex)

LookUpIndex::LookUpIndex()
{
}


LookUpIndex::LookUpIndex(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool testValuesSmaller(double v1, double v2) {
	return v1 != rUNDEF && v2 != rUNDEF && v1 >= v2;
}

bool testValuesSmallerEqual(double v1, double v2) {
	return v1 != rUNDEF && v2 != rUNDEF && v1 > v2;
}

bool testValuesEqual(double v1, double v2) {
	return v1 != rUNDEF && v2 != rUNDEF && v1 != v2;
}

bool testValuesNotEqual(double v1, double v2) {
	return v1 != rUNDEF && v2 != rUNDEF && v1 == v2;
}

bool testValuesGreaterEqual(double v1, double v2) {
	return v1 != rUNDEF && v2 != rUNDEF && v1 <= v2;
}

bool testValuesGreater(double v1, double v2) {
	return v1 != rUNDEF && v2 != rUNDEF && v1 < v2;
}


double  LookUpIndex::testColumn(double testValue, const std::vector<double> column) {
	for (int i = 0; i < column.size(); ++i) {
		double v = column[i];
		if (_testFunction(v, testValue))
			return i;
	}
	if (_operator == loLESS || _operator == loLESSEQ)
		return column.size() - 1;
	if (_operator == loGREATER || _operator == loGREATEREQ)
		return 0;
	return rUNDEF;
}

bool LookUpIndex::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	bool xchanged = false;
	std::vector<double> zcolumn;
	zcolumn.reserve(_inputRaster->size().zsize());
	PixelIterator iterTest(_testRaster, BoundingBox(), PixelIterator::fZXY);
	PixelIterator iterIn(_inputRaster);
	int count = 0;
	for (auto& value : _outputRaster) {
		while (!xchanged) {
			zcolumn.push_back(*iterTest);
			++iterTest;
			xchanged = iterTest.xchanged();
		}
		value = testColumn(*iterIn, zcolumn);
		updateTranquilizer(++count, 100);
		zcolumn.clear(); // next column
		xchanged = false; // reset flag as we are in the next column now
		++iterIn;
	}

	QVariant value;
	value.setValue<IRasterCoverage>(_outputRaster);
	logOperation(_outputRaster, _expression, { _inputRaster });
	ctx->setOutput(symTable, value, _outputRaster->name(), itRASTER, _outputRaster->resource());

	return true;

}

Ilwis::OperationImplementation *LookUpIndex::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new LookUpIndex(metaid, expr);
}

Ilwis::OperationImplementation::State LookUpIndex::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	OperationImplementation::prepare(ctx, st);
	OperationHelper::check([&]()->bool { return _inputRaster.prepare(_expression.input<QString>(0), itRASTER); },
		{ ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" });

	OperationHelper::check([&]()->bool { return _testRaster.prepare(_expression.input<QString>(1), itRASTER); },
		{ ERR_COULD_NOT_LOAD_2,_expression.input<QString>(1), "" });

	std::vector<QString> operations = { "smaller","smallerequal","equal","notequal", "greater","greaterequal" };

	OperationHelper::check([&]()->bool { return std::find(operations.begin(), operations.end(), _expression.input<QString>(2)) != operations.end(); },
		{ ERR_ILLEGAL_VALUE_2,TR("operator"),_expression.input<QString>(2) });

	QString testOperator = _expression.input<QString>(2);
	if (testOperator == "smaller") {
		_operator = loLESS;
		_testFunction = testValuesSmaller;
	}
	if (testOperator == "smallerequal") {
		_operator = loLESSEQ;
		_testFunction = testValuesSmallerEqual;
	}

	if (testOperator == "equal") {
		_operator = loEQ;
		_testFunction = testValuesEqual;
	}
	if (testOperator == "notqual") {
		_operator = loNEQ;
		_testFunction = testValuesNotEqual;
	}
	if (testOperator == "greaterequal") {
		_operator = loGREATEREQ;
		_testFunction = testValuesGreaterEqual;
	}
	if (testOperator == "greater") {
		_operator = loGREATER;
		_testFunction = testValuesGreater;
	}


	OperationHelperRaster::initialize(_inputRaster, _outputRaster, itCOORDSYSTEM | itENVELOPE | itGEOREF);
	if (!_outputRaster.isValid()) {
		ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
		return sPREPAREFAILED;
	}

	Envelope env = _inputRaster->envelope().twoD();
	_outputRaster->envelope(env);
	_outputRaster->size(_inputRaster->size().twod());

	_outputRaster->datadefRef().domain(IDomain("count"));
	std::vector<double> indexes = { 0 };
	_outputRaster->setDataDefintions(_outputRaster->datadef().domain(), indexes);

	initialize(_outputRaster->size().linearSize());
	return sPREPARED;
}

quint64 LookUpIndex::createMetadata()
{
	OperationResource operation({ "ilwis://operations/lookupindex" });
	operation.setSyntax("lookupindex(raster, testraster, operator=!smaller|smallerequal|equal|notequal|greater|greaterequal)");
	operation.setDescription(TR("Returns a raster with indexes "));
	operation.setInParameterCount({ 3 });
	operation.addInParameter(0, itRASTER, TR("rastercoverage"), TR("input rastercoverage with numeric domain"));
	operation.addInParameter(1, itRASTER, TR("test raster"), TR("multiband raster containing values that are used to test the first raster against"));
	operation.addInParameter(2, itSTRING, TR("operator"), TR("Determines how the index is choosen"));
	operation.setOutParameterCount({ 1 });
	operation.addValidation(0, 0, "domain=numericdomain");
	operation.addValidation(1, 1, "domain=numericdomain");
	operation.addOutParameter(0, itRASTER, TR("output raster"), TR("output raster with a numeric domain"));
	operation.setKeywords("filter,raster");

	mastercatalog()->addItems({ operation });
	return operation.id();
}
