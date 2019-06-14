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
#include "ilwisoperation.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "operationhelperfeatures.h"
#include "raster2polygon.h"

using namespace Ilwis;
using namespace FeatureOperations;

REGISTER_OPERATION(RasterToPolygon)

RasterToPolygon::RasterToPolygon()
{

}

RasterToPolygon::~RasterToPolygon()
{

}

RasterToPolygon::RasterToPolygon(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

void RasterToPolygon::fillLineInfo(const std::vector<double>& inputLine,
									const std::vector<double>& inputLinePrev,
								    int lineSize,
									std::vector< DirBound>& dirBoundsCurrent,
									std::vector< DirBound>& dirBoundsNext) const
{
	// calc dirBoundsCurrent with horizontal directions by comparing vertical differences of pixel rows
		// if upper pixel > lower pixel then direction is RIGHT
	for (int j = -1; j <= lineSize; ++j) {
		int x = j + 1;
		if (inputLinePrev[x] == inputLine[x])
			dirBoundsCurrent[x] = dbNONE;
		else if (inputLinePrev[x] > inputLine[x])
			dirBoundsCurrent[x] = dbRIGHT;
		else
			dirBoundsCurrent[x] = dbLEFT;
	}
	// calc dirBoundsNext with vertical directions by comparing horizontal differences of pixel columns
		// if left pixel > right pixel then direction is UP

	for (int j = 0; j <= lineSize; ++j) {
		int x = j + 1;
		if (inputLine[x - 1] == inputLine[x])
			dirBoundsNext[x] = dbNONE;
		else if (inputLine[x - 1] > inputLine[x])
			dirBoundsNext[x] = dbUP;
		else
			dirBoundsNext[x] = dbDOWN;
	}
}

byte RasterToPolygon::setPixelFlag(const std::vector< DirBound>& dirBoundsPrev, const std::vector< DirBound>& dirBoundsCurrent, const std::vector< DirBound>& dirBoundsNext, int x) const {
	byte b = 0;
	if (dirBoundsCurrent[x] != dbNONE)
		b |= 1;
	if (dirBoundsPrev[x] != dbNONE)
		b |= 2;
	if (dirBoundsCurrent[x - 1] != dbNONE)
		b |= 4;
	if (dirBoundsNext[x] != dbNONE)
		b |= 8;

	return b;
}

void RasterToPolygon::handleNodeCases(byte PixelFlag, const std::vector<double>& inputLine, const std::vector<double>& inputLinePrev, std::vector<SegBound *>& segBoundsHoriz, std::vector<SegBound *>& segBoundsVert) const {
	switch (PixelFlag)
	{
		case 0:
		{
			segBoundsHoriz[0] = 0;
			segBoundsVert[0] = 0;
		}
		break;
	}
}

bool RasterToPolygon::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	// TODO : areanumbering

	int lineSize = _inputraster->size().xsize();
	std::vector<double> inputLine( lineSize + 2, rUNDEF); // lines are 2 cells bigger as a virtual cells are added to left and right
	std::vector<double> inputLinePrev(inputLine.size(), rUNDEF); // lines are 2 cells bigger as a virtual is added to left and right
	std::vector< DirBound> dirBoundsCurrent(inputLine.size(), dbNONE);
	std::vector< DirBound> dirBoundsNext(inputLine.size(), dbNONE);
	std::vector< DirBound> dirBoundsPrev(inputLine.size(), dbNONE);
	std::vector<SegBound *> segBoundsHoriz(inputLine.size());
	std::vector<SegBound *> segBoundsVert(inputLine.size());

	inputLine.front() = inputLine.back() = rUNDEF;

	PixelIterator iter(_inputraster);
	PixelIterator iterEnd = iter.end();
	while (iter != iterEnd) {
		inputLine[iter.x() + 1] = *iter;
		if (iter.ychanged()) {
			fillLineInfo(inputLine, inputLinePrev, lineSize, dirBoundsCurrent, dirBoundsNext);

			for (int j = 0; j <= lineSize; ++j) {
				int x = j + 1;
				// pixelFlag indicates for 4 pixel boundaries where pixels are different
							//
							//          2
							//     pix  |  pix
							//          |
							//     4 ------- 1
							//          |
							//     pix  |  pix
							//          8
							//
				byte pixelFlag = setPixelFlag(dirBoundsPrev, dirBoundsCurrent, dirBoundsNext,x);
				handleNodeCases(pixelFlag, inputLine, inputLinePrev, segBoundsHoriz, segBoundsVert);
				

			}
		}

	}

	QVariant value;
	value.setValue<IFeatureCoverage>(_outputfeatures);
	logOperation(_outputfeatures, _expression);
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
	operation.setInParameterCount({ 2 });
	operation.addInParameter(0, itRASTER, TR("input rastercoverage"), TR("input rastercoverage with any domain"));
	operation.addInParameter(1, itBOOL, TR("four connected"), TR("The context for each pixel is either 4 connected or 8 connected"));
	operation.setOutParameterCount({ 1 });
	operation.addOutParameter(0, itPOINT, TR("output polygon coverage"), TR("output polygon coverage with the domain of the input map"));
	operation.setKeywords("polygon,raster, feature");
	mastercatalog()->addItems({ operation });
	return operation.id();
}

