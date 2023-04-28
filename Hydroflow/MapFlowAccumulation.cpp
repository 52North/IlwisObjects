
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
#include "blockiterator.h"
#include "pixeliterator.h"
#include "MapFlowAccumulation.h"

using namespace Ilwis;
using namespace Hydroflow;

REGISTER_OPERATION(MapFlowAccumulation)

MapFlowAccumulation::MapFlowAccumulation()
{
}


MapFlowAccumulation::MapFlowAccumulation(quint64 metaid, const Ilwis::OperationExpression& expr) : OperationImplementation(metaid, expr)
{

}

bool MapFlowAccumulation::execute(ExecutionContext* ctx, SymbolTable& symTable)
{
   if (_prepState == sNOTPREPARED)
        if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

   executeFlowAccumulation();

   bool resource = true;
   if (resource && ctx != 0) {
	   QVariant value;
	   value.setValue<IRasterCoverage>(_outRaster);
	   ctx->setOutput(symTable, value, _outRaster->name(), itRASTER, _outRaster->resource());
   }
   return resource;
}

Ilwis::OperationImplementation* MapFlowAccumulation::create(quint64 metaid, const Ilwis::OperationExpression& expr)
{
    return new MapFlowAccumulation(metaid, expr);
}

Ilwis::OperationImplementation::State MapFlowAccumulation::prepare(ExecutionContext* ctx, const SymbolTable& st)
{
	
    OperationImplementation::prepare(ctx, st);
    QString inraster = _expression.parm(0).value();
    QString outputName = _expression.parm(0, false).value();

    if (!_inRaster.prepare(inraster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2, inraster, "");
        return sPREPAREFAILED;
    }
  
    int copylist = itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF;
    _outRaster = OperationHelperRaster::initialize(_inRaster.as<IlwisObject>(), itRASTER, copylist);
    if (!_outRaster.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }

	IDomain dom;
	dom.prepare("value");
	DataDefinition def(dom);
	def.range(new NumericRange(0, 9, 1)); // directions
	_outRaster->datadefRef() = def;

    for (quint32 i = 0; i < _outRaster->size().zsize(); ++i) {
        QString index = _outRaster->stackDefinition().index(i);
        _outRaster->setBandDefinition(index, DataDefinition(dom));
    }

    if (outputName != sUNDEF)
        _outRaster->name(outputName);

  
    _xsize = _inRaster->size().xsize();
    _ysize = _inRaster->size().ysize();

    PixelIterator iterDEM = PixelIterator(_inRaster, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator iterOut = PixelIterator(_outRaster, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator inEnd = iterDEM.end();
    
    while (iterDEM != inEnd)
    {
        *iterOut = *iterDEM;
        *iterOut++;
        *iterDEM++;
    }

    return sPREPARED;
}

quint64 MapFlowAccumulation::createMetadata()
{
    OperationResource operation({ "ilwis://operations/MapFlowAccumulation" });
    operation.setSyntax("MapFlowAccumulation(inputraster)");
    operation.setDescription(TR("performs cumulative count of the number of pixels that naturally drain into outlets"));
    operation.setInParameterCount({ 1 });
    operation.addInParameter(0, itRASTER, TR("rastercoverage"), TR("input raster flow direction rastercoverage with numeric domain"));
    operation.parameterNeedsQuotes(1);
    operation.setOutParameterCount({ 1 });

	operation.addOutParameter(0, itRASTER, TR("output raster"), TR("output raster with a numeric domain"));
    operation.setKeywords("flow accumulation,raster,image processing, numeric");

    operation.checkAlternateDefinition();
    mastercatalog()->addItems({ operation });
    return operation.id();
}


////////////////////////////////////////////////////////////////////////////////////////////////

void MapFlowAccumulation::executeFlowAccumulation()
{
	_xsize = _inRaster->size().xsize();
	_ysize = _inRaster->size().ysize();

	IRasterCoverage _iterEmptyRaster;
	PixelIterator iterPos;

	quint64 copylist = itRASTERSIZE | itENVELOPE | itINTEGER;
	//  iterpos is only used for remember iteration postion 
	_iterEmptyRaster = OperationHelperRaster::initialize(_inRaster.as<IlwisObject>(), itRASTER, copylist);
	iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);

	iterDEM = PixelIterator(_inRaster, BoundingBox(), PixelIterator::fXYZ);
	iterOut = PixelIterator(_outRaster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator inEnd = iterDEM.end();

	while (iterPos != inEnd)
	{
		Pixel pxl = iterPos.position();
		if (*(iterDEM(pxl)) == 0)
			iFlowAcc(pxl);
		iterPos++;
	}



}

long MapFlowAccumulation::iFlowAcc(Pixel pxl)
{
	//For the specified cell in loaction[iRow, iCol], 
		//checks whether its neighboring cells flow to it,
		//If true, calls the function recursively, and
		//stores the accumulated flow to output flow acc. vector in location[iRow,iCol]   
		//The recursion stops when it reaches a cell that has no flow to itsself 
		//location number
		//	-------
		//	|6|7|8|
		//	-------
		//	|5| |1|
		//	-------
		//	|4|3|2|
		//	-------
		// 
	long iAcc = 1;
	bool isFlow; //determine if the neighboring cell flows to cell in location[iRow,iCol] 
	long in, jn;

	long iRow = pxl.y;
	long iCol = pxl.x;

	Pixel pospxl;
	for (int iNr = 1; iNr < 9; iNr++)
	{
		isFlow = false;
		switch (iNr)
		{
		case 1: 
		{	//East
			if (iCol != _xsize - 1)
			{
				in = iRow;
				jn = iCol + 1;
				isFlow = *(iterDEM(Pixel(jn, in, 0))) == 5;
			}
		}
			  break;
		case 2: { //South East 
			if (iCol != _xsize - 1 && iRow != _ysize - 1)
			{
				in = iRow + 1;
				jn = iCol + 1;
				isFlow = *(iterDEM(Pixel(jn, in, 0))) == 6;
			}
		}
			  break;
		case 3: {	//South
			if (iRow != _ysize - 1)
			{
				in = iRow + 1;
				jn = iCol;
				isFlow = *(iterDEM(Pixel(jn, in, 0))) == 7;
			}
		}
			  break;
		case 4: { //South West
			if (iCol != 0 && iRow != _ysize - 1)
			{
				in = iRow + 1;
				jn = iCol - 1;
				isFlow = *(iterDEM(Pixel(jn, in, 0))) == 8;
			}
		}
			  break;
		case 5: {	//West
			if (iCol != 0)
			{
				in = iRow;
				jn = iCol - 1;
				isFlow = *(iterDEM(Pixel(jn, in, 0))) == 1;
			}
		}
			  break;
		case 6: {	//North West 
			if (iCol != 0 && iRow != 0)
			{
				in = iRow - 1;
				jn = iCol - 1;
				isFlow = *(iterDEM(Pixel(jn, in, 0))) == 2;
			}
		}
			  break;
		case 7: {	//North
			if (iRow != 0)
			{
				in = iRow - 1;
				jn = iCol;
				isFlow = *(iterDEM(Pixel(jn, in, 0))) == 3;
			}
		}
			  break;
		case 8: {	//North East
			if (iCol != _xsize - 1 && iRow != 0)
			{
				in = iRow - 1;
				jn = iCol + 1;
				isFlow = *(iterDEM(Pixel(jn, in, 0))) == 4;
			}
		}
			  break;
		}

		pospxl.x = jn;
		pospxl.y = in;
		pospxl.z = 0;

		if (isFlow)
			iAcc += iFlowAcc(pospxl);
		*(iterOut(pxl)) = iAcc;
	}
	return iAcc;

}
