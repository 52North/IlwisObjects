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
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "itemdomain.h"
#include "interval.h"
#include "intervalrange.h"
#include "table.h"
#include "PercentileGroupsColumn.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(PercentileGroupsColumn)

PercentileGroupsColumn::PercentileGroupsColumn()
{
}


PercentileGroupsColumn::PercentileGroupsColumn(quint64 metaid, const Ilwis::OperationExpression &expr) :
	OperationImplementation(metaid, expr)
{

}

bool PercentileGroupsColumn::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	std::vector<double> zcolumn;
	std::vector<double> percentiles;
	zcolumn.reserve(_inputTable->recordCount());
	for (int r = 0; r < _inputTable->recordCount(); ++r) {
		double v = _inputTable->cell(_valueColumnIndex, r).toDouble();
		if (v != rUNDEF)
			zcolumn.push_back(v);
	}
	std::sort(zcolumn.begin(), zcolumn.end());
	percentiles.resize(_distributionGroups.size(), 0);
	for (int i = 0; i < _distributionGroups.size(); ++i) {
		int rank = zcolumn.size() * _distributionGroups[i] / 100.0 + 0.5;
		percentiles[i] = zcolumn[std::min((int)zcolumn.size()-1,rank)];
	}
	int pIndex = _outputTable->columnIndex("percentile");
	int piIndex = _outputTable->columnIndex("percentileIndex");
	int count = 0;
	for (int r = 0; r < _inputTable->recordCount(); ++r) {
		double v = _inputTable->cell(_valueColumnIndex, r).toDouble();
		for (int j = 0; j < percentiles.size(); ++j) {
			if (percentiles[j] > v) {
				_outputTable->setCell(pIndex, r, percentiles[j]);
				_outputTable->setCell(piIndex, r, j);
				break;
			}
		}
		updateTranquilizer(++count, 10);
	}

	if (!_createNewTable) {
		ctx->_additionalInfo[INPUTISOUTPUTFLAG] = _inputTable->id();
	}

	QVariant out;
	out.setValue<ITable>(_outputTable);
	logOperation(_outputTable, _expression, { _inputTable });
	ctx->setOutput(symTable, out, _outputTable->name(), itTABLE, _outputTable->resource());


	return true;

}

OperationImplementation *PercentileGroupsColumn::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new PercentileGroupsColumn(metaid, expr);
}

OperationImplementation::State PercentileGroupsColumn::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	OperationImplementation::prepare(ctx, st);

	OperationHelper::check([&]()->bool { return _inputTable.prepare(_expression.input<QString>(0), itTABLE); },
		{ ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" });

	auto stops = _expression.input<QString>(2).split(",");
	double rmin = rUNDEF;

	QString cname = _expression.input<QString>(1);
	_valueColumnIndex = _inputTable->columnIndex(cname);
	
	if (_valueColumnIndex == iUNDEF) {
		kernel()->issues()->log(QString(TR("Column doesnt exist ")) + cname);
		return sPREPAREFAILED;
	}

	if (_inputTable->columndefinition(_valueColumnIndex).datadef().domain()->ilwisType() != itNUMERICDOMAIN) {
		kernel()->issues()->log(QString(TR("Value column must have numeric domain")));
		return sPREPAREFAILED;
	}

	for (auto stop : stops) {
		bool ok;
		double v = stop.toDouble(&ok);
		if (!ok) {
			kernel()->issues()->log(TR("distribution group has illegal bounds"));
			return sPREPAREFAILED;
		}
		if (rmin != rUNDEF) {
			if (v < rmin) {
				kernel()->issues()->log(TR("distribution group has illegal bounds"));
				return sPREPAREFAILED;
			}
		}
		rmin = Ilwis::min(v, rmin);

		_distributionGroups.push_back(v);
	}
	if (_distributionGroups.back() != 100)
		_distributionGroups.push_back(100);
	
	_createNewTable = _expression.input<bool>(3);
	_outputTable = _createNewTable ? _inputTable->copyTable(sUNDEF) : _inputTable;
	_outputTable->addColumn("percentile", "value");
	_outputTable->addColumn("percentileIndex", "count");

	initialize(_inputTable->recordCount());

	return sPREPARED;
}

quint64 PercentileGroupsColumn::createMetadata()
{
	OperationResource operation({ "ilwis://operations/percentilegroupscolumn" });
	operation.setSyntax("percentilegroupscolumn(inputraster, distributionGroups, sizeControlGroup,yes|!no)");
	operation.setLongName(TR("Percentile Column"));
	operation.setDescription(TR("maps each value in the column of a percentile value based on the distribution group"));
	operation.setInParameterCount({ 4 });
	operation.addInParameter(0, itTABLE, TR("input table"), TR("multiband rastercoverage with domain value"));
	operation.addInParameter(1, itSTRING, TR("value column"), TR("Column over which the frequency distribution is calculated"), OperationResource::ueCOMBO);
	operation.addValidation(0, 1, "columns with domain=numericdomain");
	operation.addInParameter(2, itSTRING, TR("distribution groups"), TR("Groups bounds over whitch the values will be distributed by percentile"));
	operation.addInParameter(3, itBOOL, TR("Create new table"), TR("The column is either in a new table or in the original table"));

	operation.setOutParameterCount({ 1 });
	operation.addOutParameter(0, itTABLE, TR("table"), sUNDEF);
	operation.setKeywords("table, classification, statistics");
	operation.parameterNeedsQuotes(1);

	mastercatalog()->addItems({ operation });
	return operation.id();
}
