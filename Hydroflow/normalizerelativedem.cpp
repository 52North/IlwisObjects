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

//#include <functional>
//#include <future>
//#include <algorithm>
//#include <cmath>
//#include <vector>
#include "kernel.h"
#include "raster.h"
#include "rastercoverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "featureiterator.h"
#include "vertexiterator.h"
#include "normalizerelativedem.h"

using namespace Ilwis;
using namespace Hydroflow;

REGISTER_OPERATION(NormalizeRelativeDEM)

Ilwis::OperationImplementation *NormalizeRelativeDEM::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new NormalizeRelativeDEM(metaid, expr);
}

NormalizeRelativeDEM::NormalizeRelativeDEM()
{
}

NormalizeRelativeDEM::NormalizeRelativeDEM(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

Ilwis::OperationImplementation::State NormalizeRelativeDEM::prepare(ExecutionContext *, const SymbolTable & )
{
    QString dem = _expression.parm(0).value();
//    QString outputName = _expression.parm(0,false).value();

    // open the input DEM
    if (!_inputDEM.prepare(dem, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2, dem, "");
        return sPREPAREFAILED;
    }

    // Open the catchment feature map
    QString catchment = _expression.parm(1).value();
    if (!_inputCatchment.prepare(catchment, itPOLYGON)){
        ERROR2(ERR_COULD_NOT_LOAD_2, catchment, "");
        return sPREPAREFAILED;
    }

    _outputObj = OperationHelperRaster::initialize(_inputDEM, itRASTER, itGEOREF | itCOORDSYSTEM | itRASTERSIZE | itBOUNDINGBOX | itENVELOPE);
    if ( !_outputObj.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }

    return sPREPARED;
}

bool NormalizeRelativeDEM::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    IRasterCoverage outputRaster = _outputObj.as<RasterCoverage>();
    IRasterCoverage dem = _inputDEM.as<RasterCoverage>();

    long count = 0;
    initialize(_inputCatchment->featureCount(itPOLYGON));

    IDomain dom;
    dom.prepare("value");
    DataDefinition def(dom);
    def.range(new NumericRange(-1, 1, 0));  // actually values are not expected to be negative, but still some are
    outputRaster->datadefRef() = def;

    // set all values in output raster to undefined
    PixelIterator flat(outputRaster);
    std::fill(flat, flat.end(), rUNDEF);

    // for all polygons in the catchment map
    // find max of input map per polygon
    // set output value in current catchment polygon to input value divided by max
    for (auto feature : _inputCatchment){
        if (feature->geometryType() == itPOLYGON) {
            PixelIterator iterSel(dem, feature->geometry());
            PixelIterator iterOut(outputRaster, feature->geometry());
            Pixel begin = iterSel.position();
            PixelIterator iterMax = std::max_element(iterSel, iterSel.end());
            iterSel[begin];
            double maxHeight = *iterMax;
            std::transform(iterSel, iterSel.end(), iterOut, [maxHeight] (const double d) { if (d == rUNDEF) return d; else return d / maxHeight; });

        }
        updateTranquilizer(++count, 1); // count per feature, not by pixel
    }

    trq()->inform("\nWriting...\n");
    trq()->stop();

    bool resource = true;
    if ( resource && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(outputRaster);
        ctx->setOutput(symTable, value, outputRaster->name(), itRASTER, outputRaster->resource() );
    }
    return resource;
}

quint64 NormalizeRelativeDEM::createMetadata()
{
    OperationResource operation({"ilwis://operations/normalizerelativedem"});
    operation.setLongName("Normalize relative height above drainage");
    operation.setSyntax("normalizerelativedem(inputrelativedem,inputcatchment)");
    operation.setDescription(TR("Normalize the relative height towards the closest drainage from ridge (min. zero) to drainage (one)"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER, TR("DEM"),TR("Input map with absolute heights"));
    operation.addInParameter(1,itPOLYGON, TR("Catchment map"),TR("Input map with catchments") );
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage"), TR("Relative height"));
    operation.setKeywords("raster, DEM, flow direction, drainage, relative height, normalize");

    mastercatalog()->addItems({operation});
    return operation.id();
}
