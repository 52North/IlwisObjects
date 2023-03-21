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
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "numericrange.h"
#include "rasterstrechoperation.h"

using namespace Ilwis;
using namespace RasterOperations;

LinearStretchOperation::LinearStretchOperation()
{
}

LinearStretchOperation::LinearStretchOperation(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

Ilwis::OperationImplementation *LinearStretchOperation::create(quint64 metaid,const Ilwis::OperationExpression& expr)
{
    return new LinearStretchOperation(metaid, expr);
}

/**
 * \todo TODO: do stretching histogram
 */
bool LinearStretchOperation::stretch(IRasterCoverage toStretch)
{
    // TODO: separate histogram into own class (and move
    // certain operations to it

    int bandIx = 0;
    double fact;
    if (_limitsFrom[bandIx].second - _limitsFrom[bandIx].first <= 0)
        fact = 0;
    else
        fact = (_limitsTo.second - _limitsTo.first) / (_limitsFrom[bandIx].second - _limitsFrom[bandIx].first);
    double off = _limitsTo.first;

    PixelIterator iterInput(_inputRaster);

    std::for_each(begin(_outputRaster), end(_outputRaster), [&](double& v) {
        double vin = *iterInput;
        if (vin != rUNDEF) {
            if (vin <= _limitsFrom[bandIx].first)
                v = _limitsTo.first;
            else if (vin >= _limitsFrom[bandIx].second)
                v = _limitsTo.second;
            else
                v = fact * (vin - _limitsFrom[bandIx].first) + off;
        }
        ++iterInput;
        if (iterInput.zchanged()) {
            bandIx = iterInput.z();
            if (_limitsFrom[bandIx].second - _limitsFrom[bandIx].first <= 0)
                fact = 0;
            else
                fact = (_limitsTo.second - _limitsTo.first) / (_limitsFrom[bandIx].second - _limitsFrom[bandIx].first);
        }
    });

    return true;
}

bool LinearStretchOperation::execute(ExecutionContext *ctx,SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    bool ok = stretch(_inputRaster);

    if ( ok && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputRaster);
		logOperation(_outputRaster, _expression, { _inputRaster });
        ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->resource() );
    }
    return ok;
}

Ilwis::OperationImplementation::State LinearStretchOperation::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString intputName = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputRaster.prepare(intputName, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2, intputName, "");
        return sPREPAREFAILED;
    }

    _outputRaster = OperationHelperRaster::initialize(_inputRaster, itRASTER, itDOMAIN | itRASTERSIZE);
    if (!_outputRaster.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, outputName);
        return sPREPAREFAILED;
    }
    _outputRaster->georeference(_inputRaster->georeference());
    _outputRaster->coordinateSystem(_inputRaster->coordinateSystem());
    _outputRaster->name(outputName);

    _limitsFrom.clear();

    int parameterCount = _expression.parameterCount();
    if (parameterCount == 2) {
        double percent = _expression.parm(1).value().toDouble();
        if (percent < 0 || percent > 100) {
            ERROR2(ERR_ILLEGAL_VALUE_2, TR("parameter"), _expression.parm(1).value());
            return sPREPAREFAILED;
        }

        for (int i = 0; i < _inputRaster->size().zsize(); ++i) {
            PixelIterator iter = _inputRaster->band(i);
            NumericStatistics stats;
            stats.calculate(iter, iter.end(), NumericStatistics::pHISTOGRAM);
            double minValue = stats(NumericStatistics::pMIN, percent);
            double maxValue = stats(NumericStatistics::pMAX, percent);
            _limitsFrom.push_back(std::pair<double, double>(minValue, maxValue));
        }

        _limitsTo = std::pair<double, double>(0, 255);
    }
    else if (parameterCount == 3) {
        double lowerin = _expression.parm(1).value().toDouble();
        double upperin = _expression.parm(2).value().toDouble();
        if (upperin < lowerin) {
            ERROR2(ERR_ILLEGAL_VALUE_2, TR("parameter"), _expression.parm(1).value());
            return sPREPAREFAILED;
        }
        for (int i = 0; i < _outputRaster->size().zsize(); ++i)
            _limitsFrom.push_back(std::pair<double, double>(lowerin, upperin));
        _limitsTo = std::pair<double, double>(0, 255);
    }
    else if (parameterCount == 4) {
        double percent = _expression.parm(1).value().toDouble();
        double lowerout = _expression.parm(2).value().toDouble();
        double upperout = _expression.parm(3).value().toDouble();
        if (percent < 0 || percent > 100) {
            ERROR2(ERR_ILLEGAL_VALUE_2, TR("parameter"), _expression.parm(1).value());
            return sPREPAREFAILED;
        }
        if (upperout < lowerout) {
            ERROR2(ERR_ILLEGAL_VALUE_2, TR("parameter"), _expression.parm(2).value());
            return sPREPAREFAILED;
        }

        for (int i = 0; i < _inputRaster->size().zsize(); ++i) {
            PixelIterator iter = _inputRaster->band(i);
            NumericStatistics stats;
            stats.calculate(iter, iter.end(), NumericStatistics::pHISTOGRAM);
            double minValue = stats(NumericStatistics::pMIN, percent);
            double maxValue = stats(NumericStatistics::pMAX, percent);
            _limitsFrom.push_back(std::pair<double, double>(minValue, maxValue));
        }
        _limitsTo = std::pair<double, double>(lowerout, upperout);
    }
    else if (parameterCount == 5) {
        double lowerin = _expression.parm(1).value().toDouble();
        double upperin = _expression.parm(2).value().toDouble();
        double lowerout = _expression.parm(3).value().toDouble();
        double upperout = _expression.parm(4).value().toDouble();

        if (upperin < lowerin) {
            ERROR2(ERR_ILLEGAL_VALUE_2, TR("parameter"), _expression.parm(1).value());
            return sPREPAREFAILED;
        }
        if (upperout < lowerout) {
            ERROR2(ERR_ILLEGAL_VALUE_2, TR("parameter"), _expression.parm(3).value());
            return sPREPAREFAILED;
        }

        for (int i = 0; i < _outputRaster->size().zsize(); ++i)
            _limitsFrom.push_back(std::pair<double, double>(lowerin, upperin));
        _limitsTo = std::pair<double, double>(lowerout, upperout);
    }

    _prepState = sPREPARED;
    return _prepState;
}

/**
 * @todo add input parameter(s) to control stretch range
 * @brief LinearStretchOperation::createMetadata
 * @return the operation's resource id
 */
quint64 LinearStretchOperation::createMetadata()
{
    OperationResource operation({"ilwis://operations/linearstretch"});
    operation.setLongName("Linear stretch");
    operation.setSyntax("linearstretch(raster,percentage) | linearstretch(raster,percentage,minout,maxout) | linearstretch(raster,minin,maxin) | linearstretch(raster,minin,maxin,minout,maxout)");
    operation.setDescription(TR("re-distributes values of an input map over a wider or narrower range of values in an output map. Stretching can for instance be used to enhance the contrast in your map when it is displayed."));
    operation.setInParameterCount({2,3,4,5});
    operation.addInParameter(0, itRASTER, TR("rastercoverage to stretch"), TR("input rastercoverage with domain item or numeric"));
    operation.addInParameter(1, itNUMERIC, TR("percentage|number"), TR("Depending on the nr. of parameters used: the percentage of pixels not to be taken into account during stretching, or the minimum cutoff pixel value"));
    operation.addInParameter(2, itNUMERIC, TR("number"), TR("The maximum cutoff pixel value or the minimum value of the output range"));
    operation.addInParameter(3, itNUMERIC, TR("number"), TR("The minimum value of the output range or the maximum value of the output range"));
    operation.addInParameter(4, itNUMERIC, TR("number"), TR("The maximum value of the output range"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0, itRASTER, TR("output rastercoverage"), TR("output rastercoverage stretched"), itINTEGER | itFLOAT | itDOUBLE);
    operation.setKeywords("raster,image processing,numeric,contrast");

    operation.checkAlternateDefinition();
    mastercatalog()->addItems({operation});
    return operation.id();
}


