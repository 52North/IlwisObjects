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
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "tranquilizer.h"
#include "zonalstatistics.h"
#include <unordered_map>

using namespace Ilwis;
using namespace RasterOperations;

ZonalStatisticsBase::ZonalStatisticsBase()
{
}

ZonalStatisticsBase::ZonalStatisticsBase(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

Ilwis::OperationImplementation::State ZonalStatisticsBase::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	OperationImplementation::prepare(ctx, st);

	QString raster1 = _expression.parm(0).value();

	if (!_inputRaster1.prepare(_expression.parm(0).value(), itRASTER)) {
		ERROR2(ERR_COULD_NOT_LOAD_2, raster1, "");
		return sPREPAREFAILED;
	}
	if (_inputRaster1->datadef().domain()->ilwisType() != itNUMERICDOMAIN) {
		kernel()->issues()->log(TR("First inputmap must have a numeric domain"));
		return sPREPAREFAILED;
	}

	if (!_inputRaster2.prepare(_expression.parm(1).value(), itRASTER)) {
		ERROR2(ERR_COULD_NOT_LOAD_2, _expression.parm(1).value(), "");
		return sPREPAREFAILED;
	}
	if (!_inputRaster1->georeference()->isCompatible(_inputRaster2->georeference().ptr())) {
		kernel()->issues()->log(TR("Rasters must have similar georeferences"));
		return sPREPAREFAILED;
	}

	if (!_inputTable.prepare(_expression.parm(2).value(), itTABLE)) {
		ERROR2(ERR_COULD_NOT_LOAD_2, _expression.parm(2).value(), "");
		return sPREPAREFAILED;
	}
	QString zonals = _expression.input<QString>(3);
	_zonalColumnIndex = _inputTable->columnIndex(zonals);
	if (_zonalColumnIndex == iUNDEF) {
		kernel()->issues()->log(QString(TR("Column doesnt exist in table")) + zonals);
		return sPREPAREFAILED;
	}
	IDomain cdomain = _inputTable->columndefinitionRef(_zonalColumnIndex).datadef().domain();
	IDomain rdomain = _inputRaster2->datadef().domain();
	_needMapping = !cdomain->isEqual(rdomain.ptr()) && cdomain->ilwisType() != itNUMERICDOMAIN && rdomain->ilwisType() != itNUMERICDOMAIN; 
	if (!cdomain->isCompatibleWith(rdomain.ptr())) {
		kernel()->issues()->log(TR("Zonal raster and zonal column have incompatible domains"));
		return sPREPAREFAILED;
	}
	QString aggregationMethod = _expression.input<QString>(4).toLower();
	if (aggregationMethod == "max")
		_aggregationMethod = eMAX;
	else if (aggregationMethod == "min")
		_aggregationMethod = eMIN;
	else if (aggregationMethod == "sum")
		_aggregationMethod = eSUM;
	else if (aggregationMethod == "average")
		_aggregationMethod = eAVERAGE;
	else {
		kernel()->issues()->log(QString(TR("Aggregation method not supported")) + aggregationMethod);
		return sPREPAREFAILED;
	}

	return sPREPARED; 
}

void ZonalStatisticsBase::executeBase(std::unordered_map<quint32, std::pair<double, double>>& result) {

	std::unordered_map<quint32, double> mapping;
	if (_needMapping) {
		IDomain cdomain = _inputTable->columndefinitionRef(_zonalColumnIndex).datadef().domain();
		IDomain rdomain = _inputRaster2->datadef().domain();
		IItemDomain dom1 = rdomain.as<ItemDomain<DomainItem>>();
		IItemDomain dom2 = cdomain.as<ItemDomain<DomainItem>>();
		int pkeyIndex = _inputTable->columnIndex(_inputRaster2->primaryKey());
		for (int idx = 0; idx < _inputTable->recordCount(); ++idx) {
			const Record& rec = _inputTable->record(idx);
			mapping[rec.cell(pkeyIndex).toUInt()] = rec.cell(_zonalColumnIndex).toDouble();
		}
	}
	for (auto& m : mapping) {
		if (_aggregationMethod == eAVERAGE || _aggregationMethod == eSUM)
			result[m.first] = std::pair<double, double>{ 0,0 };
		if (_aggregationMethod == eMAX)
			result[m.first] = std::pair<double, double>{ 0,-1e308 };
		if (_aggregationMethod == eMIN)
			result[m.first] = std::pair<double, double>{ 0, 1e308 };
	}

	PixelIterator iter(_inputRaster1);
	quint64 count = 0;
	for (auto v : _inputRaster2) {
		if (v != rUNDEF) {
			auto& agg = result[(quint32)v];
			if (_aggregationMethod == eAVERAGE || _aggregationMethod == eSUM) {
				agg.first += *iter;
				agg.second++;
			}
			else if (_aggregationMethod == eMAX) {
				agg.first = std::max(*iter, agg.first);
			}
			else if (_aggregationMethod == eMIN) {
				agg.first = std::min(*iter, agg.first);
			}
		}
		updateTranquilizer(++count, 100);
		++iter;
	}


}

void ZonalStatisticsBase::createMetadata(OperationResource& operation) 
{
	operation.setInParameterCount({ 5 });
	operation.addInParameter(0, itRASTER, TR("Data rastercoverage"), TR("input rastercoverage with a domain numeric domain"));
	operation.addInParameter(1, itRASTER, TR("Zonal rastercoverage"), TR("input rastercoverage with domain an item domain equal to the column of the "));
	operation.addInParameter(2, itTABLE, TR("Zonal table"), TR("The table that contains a column with the same domain as Zonal coverage whose value are used to extract pixels. Often this is the table from the cross operation "));
	operation.addInParameter(3, itSTRING, TR("Zonal column"), TR("The column that is used to find pixels in the zonal coverage"), OperationResource::ueCOMBO);
	operation.addValidation(2, 3, "columns");
	operation.addInParameter(4, itSTRING, TR("Aggregation Method"), TR("How the pixel values are aggregated"));
	operation.setKeywords("cross,raster,table");

}

//-------------------------------------------------------------------------------
REGISTER_OPERATION(ZonalStatisticsTable)

ZonalStatisticsTable::ZonalStatisticsTable(quint64 metaid, const Ilwis::OperationExpression &expr) : ZonalStatisticsBase(metaid, expr)
{}

Ilwis::OperationImplementation *ZonalStatisticsTable::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new ZonalStatisticsTable(metaid, expr);
}

bool ZonalStatisticsTable::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	std::unordered_map<quint32, std::pair<double, double>> result;

	executeBase(result);

	std::unordered_map<quint32, quint32> _raw2record;

	for (int r = 0; r < _outputTable->recordCount(); ++r) {
		quint32 raw = _outputTable->cell(_zonalColumnIndex, r).toUInt();
		_raw2record[raw] = r;
	}
	
	initialize(_inputTable->recordCount());
	int count = 0;
	int columnIdx = _outputTable->columnIndex("zonal_statistics");
	for (auto item : result) {
		int rec = _raw2record[item.first];
		double res =item.second.first;
		if (_aggregationMethod == eAVERAGE) {
			double div = item.second.second;
			if (div != 0)
				res /= div;
			_outputTable->setCell(columnIdx, rec, res);
		}
		updateTranquilizer(++count, 10);

	}

	QVariant value;
	value.setValue<ITable>(_outputTable);
	logOperation(_outputTable, _expression, { _inputRaster1, _inputRaster2, _inputTable });
	ctx->setOutput(symTable, value, _outputTable->name(), itTABLE, _outputTable->resource());

	return true;
}

Ilwis::OperationImplementation::State ZonalStatisticsTable::prepare(ExecutionContext *ctx, const SymbolTable &st) {
	if (ZonalStatisticsBase::prepare(ctx, st) == sPREPAREFAILED)
		return sPREPAREFAILED;

	Table *tbl = static_cast<Table *>(_inputTable->clone());
	Resource res = tbl->resource();
	quint64 newId = Identity::newAnonymousId();
	QString path = INTERNAL_CATALOG + "/" + ANONYMOUS_PREFIX + QString::number(newId);
	res.setId(newId);
	res.setUrl(path, false, false);
	res.setUrl(path, true, false);
	tbl->resourceRef() = res;
	_outputTable.set(tbl);
	_outputTable->addColumn("zonal_statistics", "value");

	initialize(_inputRaster1->size().linearSize());

	return sPREPARED;
}

quint64 ZonalStatisticsTable::createMetadata()
{

	OperationResource operation({ "ilwis://operations/zonalstatisticstable" });
	operation.setLongName("Zonalstatistics table");
	operation.setSyntax("zonalstatisticstable(raster1, raster2, table, column, method=!average|sum|max|min)");
	operation.setDescription(TR("Performs an overlay of two raster maps. Extracting and aggregatin pixels from one raster based on the zonal information in the second raster. It adds a column to the input table with this info"));
	operation.setOutParameterCount({ 1 });
	operation.addOutParameter(0, itTABLE, TR("output table"), TR("output table with the results of the zonal aggregation operation in an extra column"));
	operation.setKeywords("cross,raster,table");
	ZonalStatisticsBase::createMetadata(operation);

	mastercatalog()->addItems({ operation });
	return operation.id();

}

//------------------------------------------------------------------
REGISTER_OPERATION(ZonalStatisticsRaster)

ZonalStatisticsRaster::ZonalStatisticsRaster(quint64 metaid, const Ilwis::OperationExpression &expr) : ZonalStatisticsBase(metaid, expr)
{}

Ilwis::OperationImplementation *ZonalStatisticsRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new ZonalStatisticsRaster(metaid, expr);
}

bool ZonalStatisticsRaster::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	std::unordered_map<quint32, std::pair<double, double>> result;

	executeBase(result);
	
	int count = 0;
	trq()->current(count); // start again
	PixelIterator iter2(_outputRaster);
	for (auto v : _inputRaster2) {
		if (v != rUNDEF) {
			double res = result[(quint32)v].first;
			if (_aggregationMethod == eAVERAGE) {
				double div = result[(quint32)v].second;
				if ( div != 0)
					res /= div;
			}
			*iter2 = res;
		}
		updateTranquilizer(++count, 100);
		++iter2;
	}

	_outputRaster->statistics(ContainerStatistics<double>::pBASIC);

	QVariant value;
	value.setValue<IRasterCoverage>(_outputRaster);
	logOperation(_outputRaster, _expression, { _inputRaster1, _inputRaster2, _inputTable });
	ctx->setOutput(symTable, value, _outputRaster->name(), itRASTER, _outputRaster->resource());

	return true;
}

Ilwis::OperationImplementation::State ZonalStatisticsRaster::prepare(ExecutionContext *ctx, const SymbolTable &st) {
	if (ZonalStatisticsBase::prepare(ctx, st) == sPREPAREFAILED)
		return sPREPAREFAILED;

	OperationHelperRaster::initialize(_inputRaster1, _outputRaster, itCOORDSYSTEM | itENVELOPE | itGEOREF);
	if (!_outputRaster.isValid()) {
		ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
		return sPREPAREFAILED;
	}
	Envelope env = _inputRaster1->envelope().twoD();
	_outputRaster->envelope(env);
	_outputRaster->size(_inputRaster1->size().twod());
	_outputRaster->datadefRef().domain(IDomain("value"));
	std::vector<double> indexes = { 0 };
	_outputRaster->setDataDefintions(_outputRaster->datadef().domain(), indexes);

	initialize(_inputRaster1->size().linearSize());

	return sPREPARED;
}

quint64 ZonalStatisticsRaster::createMetadata()
{
	OperationResource operation({ "ilwis://operations/zonalstatisticsraster" });
	operation.setLongName("Zonalstatistics raster");
	operation.setSyntax("zonalstatisticsraster(raster1, raster2, table, column, method=!average|sum|max|min)");
	operation.setDescription(TR("Performs an overlay of two raster maps. Extracting and aggregatin pixels from one raster based on the zonal information in the second raster. It creates a raster with the aggregtion"));
	operation.setOutParameterCount({ 1 });
	operation.addOutParameter(0, itRASTER, TR("output table"), TR("output raster with the results of the zonal aggregation"));
	operation.setKeywords("cross,raster,table");
	ZonalStatisticsBase::createMetadata(operation);

	mastercatalog()->addItems({ operation });
	return 	operation.id();
}
