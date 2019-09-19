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

void RasterToPolygon::neighbourvalues(PixelIterator& inIter, PixelIterator& handledIterr, std::vector<PIXVALUETYPE>& neighbours) {

}


int RasterToPolygon::countNeighbours(BlockIterator& inIter, PIXVALUETYPE refValue) {
	DoubleVector3D values = (*inIter).to3DVector();
	int nNeighbours = 0;
	for (int y = 0; y < values[0].size(); ++y) {
		for (int x = 0; x < values[0][y].size(); ++x) {
				if (values[0][y][x] != refValue) {
					++nNeighbours;
				}
		}
	}
	return nNeighbours;
}

bool RasterToPolygon::addLine(const R2PPoint& p1, const R2PPoint& p2, R2PRing& ring) const{
	bool empty = ring._points.size() == 0;
	bool added = false;
	if (!empty) {
		if (p1 == ring._points.back()) {
			ring._points.push_back(p2);
			added = true;
		}
	}
	else {
		ring._points.push_back(p1);
		ring._points.push_back(p2);
		added = true;

	}
	return added;
};

bool RasterToPolygon::execute(ExecutionContext *ctx, SymbolTable& symTable) {
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	BlockIterator iterIn(_inputraster, Size<>(3, 3, 1), BoundingBox(), { 1,1,1}, true);
	PixelIterator neighbourCountIter(_neighbourCount);
	//BlockIterator iterHandled(_handledPixels, Size<>(5, 5, 1), BoundingBox(), Size<>(), true);
	auto endPos = iterIn.end();
	auto begin = _inputraster->begin();
	//(*iterHandled)(0, 0, 0) = HANDLED; // by definition the start pixel is handled

	R2PRing ring;
	while (true) {
		auto data = (*iterIn).to3DVector();
		PIXVALUETYPE vRef = (*iterIn)(0,0,0);
		std::pair<double, double> center; //
		center.first = iterIn.x() + 0.5;
		center.second = iterIn.y() + 0.5;
		int shiftx = 0; int shifty = 0;
		if (data[0][2][1] != vRef) { // bottom
			addLine({ center.first + 0.5, center.second + 0.5 }, { center.first - 0.5, center.second + 0.5 }, ring);
		}
		if (data[0][1][1] != vRef) { // left
			addLine({ center.first - 0.5, center.second + 0.5 }, { center.first - 0.5, center.second - 0.5 }, ring);
		} if ( data[0][0][1] != vRef) // top
			addLine({ center.first - 0.5, center.second - 0.5 }, { center.first + 0.5, center.second - 0.5 }, ring);
		if (data[0][1][2] != vRef) { // right
			addLine({ center.first + 0.5, center.second - 0.5 }, { center.first + 0.5, center.second + 0.5 }, ring);
		}

		iterIn = Pixel(iterIn.x() + shiftx, iterIn.y() + shifty,0);
	}


	QVariant value;
	value.setValue<IFeatureCoverage>(_outputfeatures);
	logOperation(_outputfeatures, _expression, { _inputraster });
	ctx->setOutput(symTable, value, _outputfeatures->name(), itFEATURE, _outputfeatures->resource());
	return true;
}

void RasterToPolygon::shifts(const DoubleVector3D& data, BlockIterator& iterIn) {
	// we follow clockwise
	PIXVALUETYPE vRef;
	int shiftx = 0; int shifty = 0;
	if (data[0][0][1] == vRef) {
		shifty = -1;
		if (data[0][0][0] == vRef)
			shiftx = -1;
	}
	else if (data[0][1][1] == vRef) {

	}

	
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
	OperationHelperRaster::initialize(_inputraster, _handledPixels, itGEOREF | itCOORDSYSTEM | itENVELOPE);
	std::vector<double> indexes = { 0 };
	_handledPixels->setDataDefintions(IDomain("integer"), indexes);
	_handledPixels->name("PixelSelection");

	OperationHelperRaster::initialize(_inputraster, _neighbourCount, itGEOREF | itCOORDSYSTEM | itENVELOPE);
	_handledPixels->setDataDefintions(IDomain("integer"), indexes);
	_handledPixels->name("NeighbourCount");

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

