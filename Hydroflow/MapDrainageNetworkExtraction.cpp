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
#include "MapDrainageNetworkExtraction.h"

using namespace Ilwis;
using namespace Hydroflow;


DrainageNetworkExtractionBase::DrainageNetworkExtractionBase()
{
}


DrainageNetworkExtractionBase::DrainageNetworkExtractionBase(quint64 metaid, const Ilwis::OperationExpression& expr) : OperationImplementation(metaid, expr)
{

}

bool DrainageNetworkExtractionBase::execute(ExecutionContext* ctx, SymbolTable& symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	quint64 copylist = itRASTERSIZE | itENVELOPE | itINTEGER;
	_iterEmptyRaster = OperationHelperRaster::initialize(_inRaster.as<IlwisObject>(), itRASTER, copylist);

	bool resource = executeDrainageNetworkExtraction();

	if (resource && ctx != 0) {
		QVariant value;
		value.setValue<IRasterCoverage>(_outRaster);
		ctx->setOutput(symTable, value, _outRaster->name(), itRASTER, _outRaster->resource());
	}
	return resource;
}

Ilwis::OperationImplementation* DrainageNetworkExtractionBase::create(quint64 metaid, const Ilwis::OperationExpression& expr)
{
	return new DrainageNetworkExtractionBase(metaid, expr);
}


quint64 DrainageNetworkExtractionBase::createMetadata(OperationResource& operation)
{
	operation.checkAlternateDefinition();
	mastercatalog()->addItems({ operation });
	return operation.id();
}


bool DrainageNetworkExtractionBase::executeDrainageNetworkExtraction()
{

	PixelIterator iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator iterDEM = PixelIterator(_inRaster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator iterOut = PixelIterator(_outRaster, BoundingBox(), PixelIterator::fXYZ);

	PixelIterator inEnd = iterDEM.end();

	PixelIterator iterTshd;
	PixelIterator iterFlow;

	std::fill(iterOut, iterOut.end(), 0);

	if (m_tmMethods == tmMap)
	{
		iterTshd = PixelIterator(_inThreshdRaster, BoundingBox(), PixelIterator::fXYZ);
		iterFlow = PixelIterator(_inFldRaster, BoundingBox(), PixelIterator::fXYZ);
		if (!_inThreshdRaster.isValid() || !_inFldRaster.isValid())
			return false;
	}

	while (iterPos != inEnd)
	{
		Pixel pxl = iterPos.position();
		pxl.z = 0;
		if ( *iterDEM(pxl) == iUNDEF || ( m_tmMethods == tmMap && *iterTshd(pxl) == iUNDEF ) )
		{
			*iterOut(pxl) = 0;
		}
		else if (m_tmMethods == tmMap && *iterDEM(pxl) > *iterTshd(pxl) )
		{
			*iterOut(pxl) = 2;
		}
		else if (m_tmMethods == tmValue && *iterDEM(pxl) > m_iThreshold)
		{
			*iterOut(pxl) = 2;
		}
		else
			*iterOut(pxl) = 1;
		iterPos++;
	}

	
	//Construct dead-end drainage lines, if stream threshold map is used
	if (m_tmMethods == tmMap)
	{
		Pixel pospxl;
		PixelIterator iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);
		while (iterPos != inEnd)
		{
			Pixel pxl = iterPos.position();
			if (!IsEdgeCell(pxl) && *iterOut(pxl) == 2 && *iterFlow(pxl) != 0)
			{
				
				pospxl = pxl;
				pospxl.z = 0;
				while (IsDeadendLine(pospxl))
				{
					*iterOut(pospxl) = 2;
				}
			}

			iterPos++;
		}
	}

	iterOut = PixelIterator(_outRaster, BoundingBox(), PixelIterator::fXYZ);
	while (iterOut != inEnd)
	{
		*iterOut = (*iterOut != 0) ? (*iterOut - 1) : rUNDEF;
		++iterOut;
	}

	return true;
}


bool DrainageNetworkExtractionBase::IsEdgeCell(Pixel pxl)
{
	if (pxl.y == 0 || pxl.y == _ysize - 1 ||
		pxl.x == 0 || pxl.x == _xsize - 1)
		return true;
	else
		return false;
}

bool DrainageNetworkExtractionBase::IsDeadendLine(Pixel& pxl)
{
	PixelIterator iterOut = PixelIterator(_outRaster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator iterFlow = PixelIterator(_inFldRaster, BoundingBox(), PixelIterator::fXYZ);
	
	int iFlow = *iterFlow(pxl);
	long in, jn;
	bool fCondition;
	Pixel pospxl;
	pospxl.z = 0;
	switch (iFlow)
	{
		case 1: 
		{
			pospxl.y = pxl.y;
			pospxl.x = pxl.x + 1;
			fCondition = (*iterOut(pospxl) != 2);
			break;
		}
		case 2: 
		{
			pospxl.y = pxl.y + 1;
			pospxl.x = pxl.x + 1;
			fCondition = (*iterOut(pospxl) != 2);
			break;
		}
		case 3: 
		{
			pospxl.y = pxl.y + 1;
			pospxl.x = pxl.x;
			fCondition = (*iterOut(pospxl) != 2);
			break;
		}
		case 4: 
		{
			pospxl.y = pxl.y + 1;
			pospxl.x = pxl.x - 1;
			fCondition = (*iterOut(pospxl) != 2);
			break;
		}
		case 5: 
		{
			pospxl.y = pxl.y;
			pospxl.x = pxl.x - 1;
			fCondition = (*iterOut(pospxl) != 2);
			break;
		}
		case 6: 
		{
			pospxl.y = pxl.y - 1;
			pospxl.x = pxl.x - 1;
			fCondition = (*iterOut(pospxl) != 2);
			break;
		}
		case 7: 
		{
			pospxl.y = pxl.y - 1;
			pospxl.x = pxl.x;
			fCondition = (*iterOut(pospxl) != 2);
			break;
		}
		case 8: 
		{
			pospxl.y = pxl.y - 1;
			pospxl.x = pxl.x + 1;
			fCondition = (*iterOut(pospxl) != 2);
			break;
		}
		default:
			pospxl.y = pxl.y;
			pospxl.x = pxl.x;
			fCondition = false;
	}

	pxl.y = pospxl.y;
	pxl.x = pospxl.x;

	if ((*iterOut(pospxl) == 0) || (IsEdgeCell(pospxl)))
		fCondition = false;

	return fCondition;
}


Ilwis::OperationImplementation::State DrainageNetworkExtractionBase::prepare(ExecutionContext* ctx, const SymbolTable& st)
{
	OperationImplementation::prepare(ctx, st);
	QString inraster0 = _expression.parm(0).value();
	QString outputName = _expression.parm(0, false).value();

	if (!_inRaster.prepare(inraster0, itRASTER)) {
		ERROR2(ERR_COULD_NOT_LOAD_2, inraster0, "");
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
	def.range(new NumericRange(0, 1, 1)); // directions
	_outRaster->datadefRef() = def;

	//dom.prepare("boolean");
	//DataDefinition def(dom);
	//_outRaster->datadefRef() = def;

	for (quint32 i = 0; i < _outRaster->size().zsize(); ++i) {
		QString index = _outRaster->stackDefinition().index(i);
		_outRaster->setBandDefinition(index, DataDefinition(dom));
	}

	if (outputName != sUNDEF)
		_outRaster->name(outputName);

	_xsize = _inRaster->size().xsize();
	_ysize = _inRaster->size().ysize();

	return sPREPARED;
}


REGISTER_OPERATION(DrainageNetworkExtraction)
DrainageNetworkExtraction::DrainageNetworkExtraction(quint64 metaid, const Ilwis::OperationExpression& expr) : DrainageNetworkExtractionBase(metaid, expr)
{}


Ilwis::OperationImplementation* DrainageNetworkExtraction::create(quint64 metaid, const Ilwis::OperationExpression& expr)
{
	return new DrainageNetworkExtraction(metaid, expr);
}

Ilwis::OperationImplementation::State DrainageNetworkExtraction::prepare(ExecutionContext* ctx, const SymbolTable& st)
{

	if (DrainageNetworkExtractionBase::prepare(ctx, st) == sPREPAREFAILED)
		return sPREPAREFAILED;

	m_iThreshold = _expression.parm(1).value().toInt();


	m_tmMethods = tmValue;

	return sPREPARED;
}

quint64 DrainageNetworkExtraction::createMetadata()
{
	OperationResource operation({ "ilwis://operations/ExtractDrainageUseThresholdValue" });
	operation.setSyntax("ExtractDrainageUseThresholdValue(FlowAccumulationRaster,thresholdval)");
	operation.setDescription(TR("Extract a basic drainage network raster using a threshold value"));
	operation.setInParameterCount({ 2 });
	operation.addInParameter(0, itRASTER, TR("rastercoverage"), TR("flow accumulation raster"));
	operation.addInParameter(1, itINT32, TR("threshold value"), TR("a value for the minimum number of pixels that should drain into the output drainage network"));

	operation.parameterNeedsQuotes(1);
	operation.setOutParameterCount({ 1 });
	operation.addOutParameter(0, itRASTER, TR("output raster"), TR("output raster containg the extracted drainage network"));
	operation.setKeywords("rasters,drainage network,numeric");

	return DrainageNetworkExtractionBase::createMetadata(operation);

}



REGISTER_OPERATION(DrainageNetworkExtractionWithStreamThreshold)
DrainageNetworkExtractionWithStreamThreshold::DrainageNetworkExtractionWithStreamThreshold(quint64 metaid, const Ilwis::OperationExpression& expr) : DrainageNetworkExtractionBase(metaid, expr)
{}


Ilwis::OperationImplementation* DrainageNetworkExtractionWithStreamThreshold::create(quint64 metaid, const Ilwis::OperationExpression& expr)
{
	return new DrainageNetworkExtractionWithStreamThreshold(metaid, expr);
}

Ilwis::OperationImplementation::State DrainageNetworkExtractionWithStreamThreshold::prepare(ExecutionContext* ctx, const SymbolTable& st)
{
	if (DrainageNetworkExtractionBase::prepare(ctx, st) == sPREPAREFAILED)
		return sPREPAREFAILED;

	QString inraster1 = _expression.parm(1).value();
	QString inraster2 = _expression.parm(2).value();

	if (!_inThreshdRaster.prepare(inraster1, itRASTER)) {
		ERROR2(ERR_COULD_NOT_LOAD_2, inraster1, "");
		return sPREPAREFAILED;
	}

	if (!_inFldRaster.prepare(inraster2, itRASTER)) {
		ERROR2(ERR_COULD_NOT_LOAD_2, inraster2, "");
		return sPREPAREFAILED;
	}

	// Check if we are dealing with FlowDirection.dom; if yes, then recalc raw vals of flow direction map. We also allow Value maps with values 0 til 8 (0 = flat / undef, 1 til 8 are the directions).
	IDomain itemdom = _inFldRaster->datadefRef().domain();
	if (itemdom.isValid() && hasType(itemdom->valueType(), itTHEMATICITEM | itNUMERICITEM | itTIMEITEM | itNAMEDITEM)) {
		_inFldRaster.set(_inFldRaster->clone());
		PixelIterator iterFld = PixelIterator(_inFldRaster, BoundingBox(), PixelIterator::fXYZ);
		PixelIterator iterFldEnd = iterFld.end();
		while (iterFld != iterFldEnd)
		{
			*iterFld = (*iterFld != rUNDEF) ? (*iterFld + 1) : 0; // shift the values, to make them the same as ilwis3 (0 is undef, 1..8 are the direction)
			++iterFld;
		}
	}

	m_tmMethods = tmMap;
	return sPREPARED;

}


quint64 DrainageNetworkExtractionWithStreamThreshold::createMetadata()
{
	OperationResource operation({ "ilwis://operations/ExtractDrainageUseThresholdRaster" });
	operation.setSyntax("ExtractDrainageUseThresholdRaster(inputraster1,inputraster2,inputraster3)");
	operation.setDescription(TR("Extract a basic drainage network raster using a threshold raster containing various stream threshold values"));
	operation.setInParameterCount({ 3 });
	operation.addInParameter(0, itRASTER, TR("rastercoverage"), TR("input flow accumulation raster"));
	operation.addInParameter(1, itRASTER, TR("rastercoverage"), TR("input stream threshold raster"));
	operation.addInParameter(2, itRASTER, TR("rastercoverage"), TR("input flow direction raster"));
	
	operation.parameterNeedsQuotes(1);
	operation.setOutParameterCount({ 1 });
	operation.addOutParameter(0, itRASTER, TR("output raster"), TR("output raster containing the extracted drainage network"));
	operation.setKeywords("rasters,drainage network, stream threshold,numeric");

	return DrainageNetworkExtractionBase::createMetadata(operation);
}
