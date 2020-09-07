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
#include "coverage.h"
#include "raster.h"
#include "pixeliterator.h"
#include "geos/geom/Point.h"
#include "geos/geom/GeometryFactory.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "table.h"
#include "symboltable.h"
#include "blockiterator.h"
#include "itemdomain.h"
#include "ilwisoperation.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "operationhelperfeatures.h"
#include "raster2polygon.h"

using namespace Ilwis;
using namespace FeatureOperations;

//REGISTER_OPERATION(RasterToPolygon)

RasterToPolygon::RasterToPolygon()
{

}

RasterToPolygon::~RasterToPolygon()
{

}

RasterToPolygon::RasterToPolygon(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
	
}


RasterToPolygon::NetworkPoint RasterToPolygon::makeConnection(const Pixel& pCenter, bool isTemp, std::vector<RasterToPolygon::PrevLinePoint>& previousLine) {
	
	if (previousLine[pCenter.x].isValid()) {
		RasterToPolygon::NetworkPoint currentPoint;
		RasterToPolygon::PrevLinePoint newPPoint;
		currentPoint._x = pCenter.x;
		currentPoint._y = pCenter.y;
		auto topPoint = previousLine[pCenter.x];
		if (isTemp) {
			if (topPoint._shadowLink != iUNDEF) {
				newPPoint._shadowLink = topPoint._shadowLink;
			}
			else {
			}

		}
	}
	return RasterToPolygon::NetworkPoint();
}

bool RasterToPolygon::execute(ExecutionContext *ctx, SymbolTable& symTable) {
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	bool canConnect;
	NetworkPoint previousPoint;
	std::vector<PrevLinePoint> previousLine(_inputraster->size().xsize());
	BlockIterator iterBlock(_inputraster, Size<>(3, 3, 1), BoundingBox(), { 1,1,1}, true);
	auto end = iterBlock.end();
	while (iterBlock != end) {
		auto block = (*iterBlock).to3DVector();
		auto currentValue = block[1][1][0];
		auto pCenter = (*iterBlock).position();
		if (currentValue != block[0][1][0] && currentValue != block[0][0][0]) {
			// temporary points are points that do not end up in the network but are in _previousLine. As they represent a
			// intermediate point on a straight vertical line segment they have no place in the final network but are needed
			// to give the _previousline the correct linkage.
			bool  isTempPoint = block[0][0][0] != currentValue && 
								block[0][0][0] == block[0][2][0] &&
								currentValue == block[0][1][0];
			if (canConnect) {
				previousPoint = makeConnection(pCenter, isTempPoint, previousLine);
			}
			canConnect = true;
		}
		// if the topmid value equals the current value we are looking at a point in the middle of a filled area. so no lines we be drawn here
		if (block[1][1][0] == currentValue) {
			canConnect = false;
			previousPoint = NetworkPoint();
		}
	}


	QVariant value;
	value.setValue<IFeatureCoverage>(_outputfeatures);
	logOperation(_outputfeatures, _expression, { _inputraster });
	ctx->setOutput(symTable, value, _outputfeatures->name(), itFEATURE, _outputfeatures->resource());
	return true;
}

	
Ilwis::OperationImplementation *RasterToPolygon::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new RasterToPolygon(metaid, expr);
}

Ilwis::OperationImplementation::State RasterToPolygon::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	OperationImplementation::prepare(ctx, st);
	QString raster = _expression.parm(0).value();
	QString outputName = _expression.parm(0, false).value();

	if (!_inputraster.prepare(raster, itRASTER)) {

		ERROR2(ERR_COULD_NOT_LOAD_2, raster, "");
		return sPREPAREFAILED;
	}


	return sPREPARED;
}

quint64 RasterToPolygon::createMetadata()
{
	OperationResource operation({ "ilwis://operations/raster2polygon" });
	operation.setSyntax("raster2polygon(inputraster)");
	operation.setDescription(TR("translates the pixels of a rastercoverage to po;ygons in a featurecoverage"));
	operation.setInParameterCount({ 1 });
	operation.addInParameter(0, itRASTER, TR("input rastercoverage"), TR("input rastercoverage with any domain"));
	operation.setOutParameterCount({ 1 });
	operation.addOutParameter(0, itPOINT, TR("output polygon coverage"), TR("output polygon coverage with the domain of the input map"));
	operation.setKeywords("polygon,raster, feature");
	mastercatalog()->addItems({ operation });
	return operation.id();
}

