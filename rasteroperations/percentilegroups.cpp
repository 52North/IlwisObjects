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
#include "percentilegroups.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(PercentileGroups)

PercentileGroups::PercentileGroups()
{
}


PercentileGroups::PercentileGroups(quint64 metaid, const Ilwis::OperationExpression &expr) :
	OperationImplementation(metaid, expr)
{

}

bool PercentileGroups::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	std::vector<double> zcolumn;
	zcolumn.reserve(_inputRaster->size().zsize());
	BoundingBox box(Size<>(_inputRaster->size().xsize(), _inputRaster->size().ysize(), _controlGroupCount));
	PixelIterator iterIn(_inputRaster, box, PixelIterator::fZXY);
	PixelIterator iterOut(_outputRaster, BoundingBox(), PixelIterator::fZXY);
	auto endRaster = iterIn.end();
	int count = 0;
	double value = *iterIn;
	while (iterIn != endRaster) {
		if (iterIn.xchanged()) {
			if (zcolumn.size() > 0) {
				std::sort(zcolumn.begin(), zcolumn.end());
				if (_extraBoundaryPerc > 0) {
					double delta = std::abs(zcolumn.front() - zcolumn.back()) * _extraBoundaryPerc;
					zcolumn.insert(zcolumn.begin() + 0, (zcolumn.front() - delta));
					zcolumn.push_back(zcolumn.back() + delta);
				}
				std::vector<int> percRanks;
				for (auto v : _distributionGroups) {
					percRanks.push_back(1 + (v / 100.0) * (zcolumn.size()));
				}

				for (auto rank : percRanks) {
					*iterOut = rank < zcolumn.size() ? zcolumn[rank] : zcolumn.back();
					++iterOut;
				}
			}
			else
				iterOut += _distributionGroups.size();
			zcolumn.clear();
		}

		if (!isNumericalUndef(value)) {
			zcolumn.push_back(*iterIn);
		}
		updateTranquilizer(count++, 1000);
		++iterIn;
		value = *iterIn;
	}
	
	QVariant out;
	out.setValue<IRasterCoverage>(_outputRaster);
	logOperation(_outputRaster, _expression, { _inputRaster });
	ctx->setOutput(symTable, out, _outputRaster->name(), itRASTER, _outputRaster->resource());


	return true;

}

OperationImplementation *PercentileGroups::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new PercentileGroups(metaid, expr);
}

OperationImplementation::State PercentileGroups::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	OperationImplementation::prepare(ctx, st);
	QString raster = _expression.parm(0).value();

	if (!_inputRaster.prepare(raster, itRASTER)) {
		ERROR2(ERR_COULD_NOT_LOAD_2, raster, "");
		return sPREPAREFAILED;
	}
	OperationHelper::check([&]()->bool { return _inputRaster.prepare(_expression.input<QString>(0), itRASTER); },
		{ ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" });

	auto stops = _expression.input<QString>(1).split(",");
	double rmin = rUNDEF;

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

	_controlGroupCount = _expression.input<int>(2);
	if (_controlGroupCount > _inputRaster->size().zsize() - 1 && _controlGroupCount > 0) {
		kernel()->issues()->log(TR("Control group size invalid"));
		return sPREPAREFAILED;
	}
	OperationHelperRaster::initialize(_inputRaster, _outputRaster, itCOORDSYSTEM | itGEOREF | itENVELOPE);
	if (!_outputRaster.isValid()) {
		ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
		return sPREPAREFAILED;
	}
	_outputRaster->size(Size<>(_inputRaster->size().xsize(), _inputRaster->size().ysize(), _distributionGroups.size()));
	
	std::vector<double> indexes;
	for (int i = 0; i < _distributionGroups.size(); ++i) {
		indexes.push_back(i);
	}
	_outputRaster->setDataDefintions(IDomain("value"), indexes);
	initialize(_outputRaster->size().linearSize());
	return sPREPARED;
}

quint64 PercentileGroups::createMetadata()
{
	OperationResource operation({ "ilwis://operations/percentilegroups" });
	operation.setSyntax("percentilegroups(inputraster, distributionGroups, sizeControlGroup)");
	operation.setDescription(TR("maps each pixel value in the stack of a pixel to a percentile value basded on the distribution group"));
	operation.setInParameterCount({ 3 });
	operation.addInParameter(0, itRASTER, TR("input rastercoverage"), TR("multiband rastercoverage with domain value"));
	operation.addInParameter(1, itSTRING, TR("Dsitribution groups"), TR("Groups bounds over whitch the values will be distributed by percentile"));
	operation.addInParameter(2, itPOSITIVEINTEGER, TR("Control group count"), TR("Bands below this index are used for calculation of the trend numbers"));
	operation.setOutParameterCount({ 1 });
	operation.addOutParameter(0, itRASTER, TR("adjusted raster"), sUNDEF, itFLOAT | itDOUBLE);
	operation.setKeywords("raster, classification, statistics");
	operation.parameterNeedsQuotes(1);

	mastercatalog()->addItems({ operation });
	return operation.id();
}
