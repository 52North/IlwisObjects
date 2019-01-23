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
#include "operationhelpergrid.h"
#include "geometryhelper.h"
#include "aggregaterasterstatistics.h"
#include "aggregaterasterstatistics.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(AggregateRasterStatistics)

double stats_min(std::vector<double>& data) {
	double result = rUNDEF;
	for (double v : data) {
		result = std::min(v, result);
	}
	return result;
}
double stats_max(std::vector<double>& data) {
	double result = rUNDEF;
	for (double v : data) {
		result = std::max(v, result);
	}
	return result;
}
double stats_median(std::vector<double>& data) { // assume sorted
	std::sort(data.begin(), data.end());
	if ( data.size() %2 != 0)
		return data[data.size() / 2];
	else
		return 0.5 * (data[data.size() / 2 - 1] + data[data.size() / 2]);
}

double stats_max_index(const std::vector<double>& data) {
	double rmax = rUNDEF;
	double maxIndex = rUNDEF;
	for (int i = 0; i < data.size(); ++i) {
		if (data[i]!= rUNDEF) {
			if (data[i] > rmax) {
				rmax = data[i];
				maxIndex = i;
			}
		}
	}
	return maxIndex;
}
double stats_min_index(const std::vector<double>& data) {
	double rmin = 1e308;
	double minIndex = rUNDEF;
	for (int i = 0; i < data.size(); ++i) {
		if (data[i] != rUNDEF) {
			if (data[i] < rmin) {
				rmin = data[i];
				minIndex = i;
			}
		}
	}
	return minIndex;
}

double stats_mean(const std::vector<double>& data) {
	double sum = std::accumulate(data.begin(), data.end(), 0.0);
	return sum / data.size();
}

double stats_variance(const std::vector<double>& data) {
	double mean = stats_mean(data);
	double sqSum = std::inner_product(data.begin(), data.end(), data.begin(), 0.0);
	return sqSum / data.size() - mean * mean;
}

double stats_sd(const std::vector<double>& data) {
	return std::sqrt(stats_variance(data));
}
double stats_tss(const std::vector<double>& data) {
	double mean = stats_mean(data);
	double result = 0;
	for (double v : data) {
		double r = v - mean;
		result += (r * r);
	}
	return result;

}
double stats_skew(const std::vector<double>& data) {
	double sum = 0;
	double mean = stats_mean(data);
	double std = stats_sd(data);
	for (int i = 0; i < data.size(); i++) {
		double delta = data[i] - mean;
		sum += delta * delta * delta;
	}

	return sum / (data.size() * std * std * std );
}

// excess kurtosis: zero for normal distribution (Kenney, J. F. and Keeping, E. S. Mathematics of Statistics, Pt. 2, 2nd ed. Princeton, NJ: Van Nostrand, 1951)
double stats_kurtosis(const std::vector<double>& data) {
    double sum = 0;
    double mean = stats_mean(data);
    double std = stats_sd(data);
    for (int i = 0; i < data.size(); i++) {
        double delta = data[i] - mean;
        sum += delta * delta * delta * delta;
    }

    return sum / (data.size() * std * std * std * std) - 3;
}


double stats_sum(const std::vector<double>& data) {
	double sum = rUNDEF;
	for (double v : data) {
		sum += v;
	}
	return sum;
}

Ilwis::OperationImplementation *AggregateRasterStatistics::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new AggregateRasterStatistics(metaid, expr);
}

AggregateRasterStatistics::AggregateRasterStatistics()
{
}

AggregateRasterStatistics::AggregateRasterStatistics(quint64 metaid, const Ilwis::OperationExpression &expr) :
	OperationImplementation(metaid, expr)
{
}

bool AggregateRasterStatistics::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;
	double inValue;
	bool xchanged = false;
	std::vector<double> zcolumn;
	zcolumn.reserve(_inputRaster->size().zsize());
	PixelIterator iterIn(_inputRaster, BoundingBox(), PixelIterator::fZXY);
	int count = 0;
	for (auto& value : _outputRaster) {
		while (!xchanged) {
			inValue = *iterIn;
			if (_operationName == "maxindex;" || _operationName == "minindex") {
				zcolumn.push_back(inValue);
			}else if (inValue != rUNDEF)
				zcolumn.push_back(inValue);
			++iterIn;
			xchanged = iterIn.xchanged();
		}
		updateTranquilizer(++count, 100);
		value = zcolumn.size() > 0 ? _statisticsFunction(zcolumn) : rUNDEF;
		zcolumn.clear(); // next column
		xchanged = false; // reset flag as we are in the next column now
	}
	_outputRaster->statistics(ContainerStatistics<double>::pBASIC);

	QVariant value;
	value.setValue<IRasterCoverage>(_outputRaster);
	logOperation(_outputRaster, _expression);
	ctx->setOutput(symTable, value, _outputRaster->name(), itRASTER, _outputRaster->resource());

	return true;


}

Ilwis::OperationImplementation::State AggregateRasterStatistics::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	try {
		OperationImplementation::prepare(ctx, st);
		OperationHelper::check([&]()->bool { return _inputRaster.prepare(_expression.input<QString>(0), itRASTER); },
			{ ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" });

		std::vector<QString> operations = { "mean","variance","standarddev","totalsumsquares", "skew","kurtosis","max","min","maxindex","minindex","median","sum" };

		OperationHelper::check([&]()->bool { return std::find(operations.begin(), operations.end(), _expression.input<QString>(1)) != operations.end(); },
			{ ERR_ILLEGAL_VALUE_2,TR("statistical operation"),_expression.input<QString>(1) });

		_operationName = _expression.input<QString>(1);

		QString outputName = _expression.parm(0, false).value();

		OperationHelperRaster::initialize(_inputRaster, _outputRaster, itCOORDSYSTEM | itGEODETICDATUM | itGEOREF);
		if (!_outputRaster.isValid()) {
			ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
			return sPREPAREFAILED;
		}
		Envelope env = _inputRaster->envelope().twoD();
		_outputRaster->envelope(env);
		_outputRaster->size(_inputRaster->size().twod());

		if (_operationName == "median" || _operationName == "min" || _operationName == "max")
			_outputRaster->datadefRef() = DataDefinition(_inputRaster->datadef().domain(), _inputRaster->datadef().range()->clone());
		else if (_operationName == "maxindex" || _operationName == "minindex") {
			_outputRaster->datadefRef().domain(IDomain("count"));
		}
		else
			_outputRaster->datadefRef().domain(IDomain("value"));
		std::vector<double> indexes = { 0 };
		_outputRaster->setDataDefintions(_outputRaster->datadef().domain(), indexes);

		if (outputName != sUNDEF)
			_outputRaster->name(outputName);

		{
			if (_operationName == "min") {
				_statisticsFunction = stats_min;
			}
			else if (_operationName == "max") {
				_statisticsFunction = stats_max;
			}
			else if (_operationName == "median") {
				_statisticsFunction = stats_median;
			}if (_operationName == "maxindex") {
				_statisticsFunction = stats_max_index;
			}if (_operationName == "minindex") {
				_statisticsFunction = stats_min_index;
			}if (_operationName == "variance") {
				_statisticsFunction = stats_variance;
			}if (_operationName == "standarddev") {
				_statisticsFunction = stats_sd;
			}if (_operationName == "totalsumsquares") {
				_statisticsFunction = stats_tss;
			}if (_operationName == "skew") {
				_statisticsFunction = stats_skew;
			}if (_operationName == "kurtosis") {
				_statisticsFunction = stats_kurtosis;
			}if (_operationName == "mean") {
				_statisticsFunction = stats_mean;
			} if (_operationName == "sum") {
				_statisticsFunction = stats_sum;
			}
		}
		initialize(_outputRaster->size().linearSize());
		return sPREPARED;

	}
	catch (const CheckExpressionError& err) {
		ERROR0(err.message());
	}
	return sPREPAREFAILED;


	return sPREPARED;
}

quint64 AggregateRasterStatistics::createMetadata()
{
	OperationResource operation({ "ilwis://operations/aggregaterasterstatistics" }, "ilwis");
	operation.setSyntax("aggregaterasterstatistics(inputraster,statisticalmarker=mean|variance|standarddev|totalsumsquares|skew|kurtosis|max|min|maxindex|minindex|median|sum)");
	operation.setDescription(TR("aggregates the bands of a raster stack into a new rastercoverage according to the method indicated by the second parameter"));
	operation.setInParameterCount({ 2 });
	operation.addInParameter(0, itRASTER, TR("input raster"), TR("set raster bands to be aggregated"));
	operation.addInParameter(1, itSTRING, TR("statistical method"), TR("method of calucaltion for a pixel column of the stack of bands"));
	operation.setOutParameterCount({ 1 });
	operation.addOutParameter(0, itRASTER, TR("output raster"), TR("Single band raster with the aggregated statical values"));
	operation.setKeywords("raster, statistics,numeric,aggregate");

	mastercatalog()->addItems({ operation });
	return operation.id();
}
