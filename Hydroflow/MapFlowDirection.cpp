
/* IlwisObjects is a framework for analysis, processingand visualization of remote sensingand gis data
Copyright(C) 2018  52n North

This program is free software : you can redistribute it and /or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see < http://www.gnu.org/licenses/>.*/

#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "blockiterator.h"
#include "pixeliterator.h"
#include "MapFlowDirection.h"
#include "itemdomain.h"
#include "thematicitem.h"



using namespace Ilwis;
using namespace Hydroflow;

REGISTER_OPERATION(MapFlowDirection)

MapFlowDirection::MapFlowDirection()
{
}

MapFlowDirection::MapFlowDirection(quint64 metaid, const Ilwis::OperationExpression& expr) : OperationImplementation(metaid, expr)
{
}

bool MapFlowDirection::execute(ExecutionContext* ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

	executeFlowDirection();

    bool resource = true;
    if (resource && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outRaster);
        ctx->setOutput(symTable, value, _outRaster->name(), itRASTER, _outRaster->resource());
    }
    return resource;
}

Ilwis::OperationImplementation* MapFlowDirection::create(quint64 metaid, const Ilwis::OperationExpression& expr)
{
    return new MapFlowDirection(metaid, expr);
}

Ilwis::OperationImplementation::State MapFlowDirection::prepare(ExecutionContext* ctx, const SymbolTable& st)
{
	m_fParallel = false;
	m_fmMethods = fmSlope;

	OperationImplementation::prepare(ctx, st);
    QString inraster = _expression.parm(0).value();
    QString outputName = _expression.parm(0, false).value();
    QString methodstr = _expression.parm(1).value().toLower();
	QString parallelstr = _expression.parm(2).value().toLower();

    std::map<QString, FlowMethod> methods = { {"slope",fmSlope},{"height",fmHeight} };

    auto iter = methods.find(methodstr);
    if (iter == methods.end())
    {
        ERROR2(ERR_NOT_FOUND2, methodstr, TR("in method for flow dirction"));
        return sPREPAREFAILED;
    }
    m_fmMethods = iter->second;

	if (parallelstr == "yes" || parallelstr == "1")
		m_fParallel = true;
	else
		m_fParallel = false;


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

	IThematicDomain flowdirectionDom;
	flowdirectionDom.prepare();
	flowdirectionDom->addItem(new ThematicItem({ "E", "", "To the east", }, 1));
	flowdirectionDom->addItem(new ThematicItem({ "SE", "", "To the south east", }, 2));
	flowdirectionDom->addItem(new ThematicItem({ "S", "", "To the south", }, 3));
	flowdirectionDom->addItem(new ThematicItem({ "SW", "", "To the south west", }, 4));
	flowdirectionDom->addItem(new ThematicItem({ "W", "", "To the west", }, 5));
	flowdirectionDom->addItem(new ThematicItem({ "NW", "", "To the north west", }, 6));
	flowdirectionDom->addItem(new ThematicItem({ "N", "", "To the north", }, 7));
	flowdirectionDom->addItem(new ThematicItem({ "NE", "", "To the north east", }, 8));
	flowdirectionDom->name("FlowDirection");
	_outRaster->datadefRef() = DataDefinition(flowdirectionDom);

    for (quint32 i = 0; i < _outRaster->size().zsize(); ++i) {
        QString index = _outRaster->stackDefinition().index(i);
        _outRaster->setBandDefinition(index, DataDefinition(flowdirectionDom));
    }

    if (outputName != sUNDEF)
        _outRaster->name(outputName);

    PixelIterator iterDEM = PixelIterator(_inRaster, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator iterFlow = PixelIterator(_outRaster, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator inEnd = iterDEM.end();

    
	std::fill(iterFlow, iterFlow.end(), 0);

    _xsize = _outRaster->size().xsize();
    _ysize = _outRaster->size().ysize();

    // initialize tranquilizer
    initialize(_xsize * _ysize);

    return sPREPARED;
}

quint64 MapFlowDirection::createMetadata()
{
	OperationResource operation({ "ilwis://operations/MapFlowDirection" });
	operation.setSyntax("MapFlowDirection(inputraster,method=slope|height,useparalleldrainagecorrection=yes|no)");
	operation.setDescription(TR("generates a new raster containing flow directions"));
	operation.setInParameterCount({ 3 });
	operation.addInParameter(0, itRASTER, TR("rastercoverage"), TR("input a sink-free DEM with numeric domain"));
	operation.addInParameter(1, itSTRING, TR("method definition"), TR("Flow direction should be calculated according to the steepst slope or the smallest height"), OperationResource::ueCOMBO);
	operation.addInParameter(2, itSTRING, TR("Parallel drainage correction algorithm"), TR("Option of flow direction algorithm."), OperationResource::ueNONE);
	operation.parameterNeedsQuotes(1);
	operation.setOutParameterCount({ 1 });
	operation.addOutParameter(0, itRASTER, TR("output raster"), TR("output raster with a numeric domain"));
	operation.setKeywords("flow direction,raster,image processing, numeric");

	operation.checkAlternateDefinition();
	mastercatalog()->addItems({ operation });
	return operation.id();

}


void MapFlowDirection::executeFlowDirection()
{
	InitPars();
	_xsize = _inRaster->size().xsize();
	_ysize = _inRaster->size().ysize();

	quint64 copylist = itRASTERSIZE | itENVELOPE | itINTEGER;
	_flagRaster = OperationHelperRaster::initialize(_inRaster.as<IlwisObject>(), itRASTER, copylist);

	//  iterpos is only used for remember iteration postion 
	_iterEmptyRaster = OperationHelperRaster::initialize(_inRaster.as<IlwisObject>(), itRASTER, copylist);
	iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);

	// ini flag value as 0;
	iterFlowFlag = PixelIterator(_flagRaster, BoundingBox(), PixelIterator::fXYZ);
	std::fill(iterFlowFlag, iterFlowFlag.end(), 0);

	iterFlow = PixelIterator(_outRaster, BoundingBox(), PixelIterator::fXYZ);
	iterDEM = PixelIterator(_inRaster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator inEnd = iterDEM.end();

	if (m_fParallel) 
	{
		FlowDirectionAlgorithm fda(_inRaster, _outRaster, _iterEmptyRaster);
		QString method = "height";
		if (m_fmMethods == fmSlope)
			method = "slope";
		fda.calculate(method);
	}
	else 
	{
		double rMax;
		int iCout = 0;

		while (iterPos != inEnd)
		{
			Pixel pxl = iterPos.position();
			//*(iterFlow(pxl)) = 0; // initial value

			//if (rbDem[iCol] != rUNDEF) {
			//Calculate slope/height diff. dh from 8-neighbors for the specified cell
			//vValue - a vector containing slope/dh values for its 8 neighbors
			std::vector<double> vValue;
			FillArray(pxl, vValue);

			//finds positions for elements with max value
			std::vector<int> vPos;
			rMax = rFindMaxLocation(vValue, vPos, iCout);

			//examine the positions with max. value
			//perform assigning flow direction algorithm.
			*(iterFlow(pxl)) = iLookUp(rMax, iCout, vPos);

			iterPos++;
		}
		//Ste flows to the flat areas
		TreatFlatAreas();

	}

	iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);

	while (iterPos != inEnd)
	{
		Pixel pxl = iterPos.position();
		if (onEdge(pxl) || *(iterFlow(pxl)) > 8)
			*(iterFlow(pxl)) = 0;
		
		iterPos++;
	}

	// Convert from ilwis3 compatible matrix to ilwis4 compatible (0 becomes undef, and 1..8 become 0..7)

	iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);
	while (iterPos != inEnd) {
		Pixel pxl = iterPos.position();
		*iterFlow(pxl) = (*iterFlow(pxl) == 0) ? rUNDEF : (*iterFlow(pxl) - 1);
		iterPos++;
	}
}

bool MapFlowDirection::onEdge(Pixel pix) 
{
	return pix.x == 0 || pix.x == _xsize - 1 ||
		pix.y == 0 || pix.y == _ysize - 1;
}


void MapFlowDirection::FillArray(Pixel pxl, std::vector<double>& vValue)
{
	//For the specified cell[iRow, iCol], compute the slope/dh values among its 8-neighbors
	//return the calculated slope/dh in vector vValue
	//iNbRow - row number for the specified neighbor
	//iNbCol - column number for the specified neighbor
	//rCurH - elevation for the specified cell[iRow, iCol]
	//rNb8H - elevation for the specified neighbor' cell[iNbRow, iNbCol]
	//rValue - slope / height difference
	//location number
	//	-------
	//	|6|7|8|
	//	-------
	//	|5| |1|
	//	-------
	//	|4|3|2|
	//	-------
	//
	long iNbRow, iNbCol;
	double rCurH, rNb8H, rValue;
	rCurH = *(iterDEM(pxl));
	for(int i= 0; i<8; i++)
	{
		switch (i)
		{
			case 0: {	//location 1/E
				iNbRow = pxl.y;
				iNbCol = pxl.x+1;
			}
			break;
			case 1: { //location 2/SE
				iNbRow = pxl.y+1;
				iNbCol = pxl.x+1;
			}
			break;
			case 2: {	//location 3/S
				iNbRow = pxl.y+1;
				iNbCol = pxl.x;
			}
			break;
			case 3:{ //Location 4/SW
				iNbRow = pxl.y+1;
				iNbCol = pxl.x-1;
			}
			break;
			case 4:{	//location 5/W
				iNbRow = pxl.y;
				iNbCol = pxl.x-1;
			}
			break;
			case 5:{	//location 6/NW
				iNbRow = pxl.y-1;
				iNbCol = pxl.x-1;
			}
			break;
			case 6:{	//location 7/N
				iNbRow = pxl.y-1;
				iNbCol = pxl.x;
			}
			break;
			case 7:{	//location 8/NE
				iNbRow = pxl.y-1;
				iNbCol = pxl.x+1;
			}
			break;
		}

		Pixel pxlnb;
		pxlnb.y = iNbRow;
		pxlnb.x = iNbCol;
		pxlnb.z = 0;
		
		rNb8H = *(iterDEM(pxlnb));
		if (rNb8H == rUNDEF)
			vValue.push_back(rUNDEF); //never flow to undefied cell
		else{
			switch(m_fmMethods)
			{
				case fmSlope:
					rValue = rComputeSlope(rCurH, rNb8H,i+1);
					break;
				case fmHeight:
					rValue = rComputeHeightDifference(rCurH,rNb8H);
					break;
			}
			vValue.push_back(rValue);
		}
	}
}

double MapFlowDirection::rComputeSlope(double rCurH, double rNbH, int iPos)
{
		//The slope is calculated by subscribing the neighbor's value from the center
		//distance 1.14 is concerned for diagonal cells
		double rVal;
		if (isEven(iPos))
			rVal = (rCurH - rNbH)/1.41;
		else
			rVal = rCurH - rNbH;
		return rVal;
}

double MapFlowDirection::rComputeHeightDifference(double rCurH, double rNbH)
{
		return rCurH - rNbH;
}

bool MapFlowDirection::isEven(int elem)
{
		return elem % 2 == 0;
}


bool MapFlowDirection::isInOneEdge(int iPos1, int iPos2, int iPos3, std::vector<int>& vPos)
{
		bool fCondition1 = find(vPos.begin(),vPos.end(),iPos1) != vPos.end();
		bool fCondition2 = find(vPos.begin(),vPos.end(),iPos2) != vPos.end();
		bool fCondition3 = find(vPos.begin(),vPos.end(),iPos3) != vPos.end();
		return fCondition1 && fCondition2 && fCondition3;
}

void MapFlowDirection::TreatFlatAreas()
{
		std::fill(iterFlowFlag, iterFlowFlag.end(), 0);
		iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);
		PixelIterator inEnd = iterDEM.end();

		/*---For each area in the flat areas:
		  ---Identify outlets in the flat area.
		  ---Examine the neighbors in the flat area to the outlets.
		  ---If a neighbor is in the flat area and
		     has not been assigned a flow direction, its flow is set to the outlet.
		  ---Locate neighbors without flow direction, in the flat area,
			---to the neighbors that was evaluated in the previous step.
			---Set their flows to the neighbors with flow direction.
			---The procedure is repeated untill all the cells in
			---the flat area have been evaluated.
		*/

		Pixel rcInitFlat;
		m_ContFlat = 0;

		while (iterPos != inEnd)
		{
			Pixel pxl = iterPos.position();
			if ( !onEdge(pxl) && *(iterFlow(pxl)) == 9) //cell in flat area
			{
				std::vector<Pixel> vOutlets;
				vOutlets.resize(0);

				m_vFlat.resize(0);
				rcInitFlat.y = pxl.y;
				rcInitFlat.x = pxl.x;
				rcInitFlat.z = 0;

				LocateOutlets(rcInitFlat, vOutlets);

				if (vOutlets.size() > 0)
				{
					SetFlowsInFlatArea(vOutlets);
				}
				else
				{
					for (std::vector<Pixel>::iterator pos = m_vFlat.begin();
						pos < m_vFlat.end(); ++pos)
					{
						*(iterFlow(*pos)) = 0;  //make it UNDEF
					}
				}
			}
			iterPos++;
		}

}

void MapFlowDirection::LocateOutlets(Pixel rcInitFlat, std::vector<Pixel>& vOutlets)
{
		//--Locate and Flag a continous flat area to m_vFlag
		//--Locate and push the outlets to the flat area to vOutlets

		std::vector<Pixel> vNbs;
		std::vector<Pixel> vStarters;
		vStarters.push_back(rcInitFlat);

		double rHeight = *(iterDEM(rcInitFlat));
		m_ContFlat++;

		do
		{
			vNbs.resize(0);
			for (std::vector<Pixel>::iterator pos = vStarters.begin();pos < vStarters.end(); ++pos)
			{
				Pixel rcNb;
				rcNb.z = 0;
				*(iterFlowFlag(*pos)) = m_ContFlat;
				m_vFlat.push_back(*pos);

				for (int i=-1; i<=1; ++i) 			//Evaluate neighbors in flat area
				{
					rcNb.y = pos->y + i;
					for(int j=-1; j<=1; ++j)
					{
						rcNb.x = pos->x + j;

						if (*(iterFlowFlag(rcNb)) != m_ContFlat) //has not been flagged
						{
							if (*(iterFlow(rcNb)) == 9) 				 //in flat area
							{
								*(iterFlowFlag(rcNb)) = m_ContFlat;
									m_vFlat.push_back(rcNb);
									vNbs.push_back(rcNb);
							}
							else if( *(iterDEM(rcNb)) == rHeight &&	( *(iterFlow(rcNb)) != 0) )
							{
								*(iterFlowFlag(rcNb)) = m_ContFlat;
								vOutlets.push_back(rcNb);
							}
						}
					}
				}
			}
			vStarters.swap(vNbs);
		}while(vNbs.size() != 0);
}

void MapFlowDirection::SetFlowsInFlatArea(std::vector<Pixel>& vOutlets)
{
		std::vector<Pixel> vNbs;	//Neighbors without flow direction to neighbors
									//with flow direction in the flat area

		do
		{
			vNbs.resize(0);
			//for (vector<RowCol>::iterator pos = vOutlets.begin(); pos < vOutlets.end(); ++pos)
			std::vector<Pixel>::iterator pos = vOutlets.begin();
			for (;pos < vOutlets.end(); ++pos)
			{
				*(iterFlowFlag(*pos)) = 0;
			}
			for (pos = vOutlets.begin(); pos < vOutlets.end(); ++pos)
			{
				Pixel rcNb;
				rcNb.z = 0;
				int iNb = 0;
				for (int i=-1; i<=1; ++i) 			//Evaluate neighbors in flat area
				{
					rcNb.y = pos->y + i;
					for(int j=-1; j<=1; ++j)
					{
						rcNb.x = pos->x + j;
						if (*(iterFlow(rcNb)) == 9 && 
							m_vFlowSelf[iNb] != *(iterFlow(*pos)) &&
							*(iterFlowFlag(rcNb)) == m_ContFlat)

						{
							*(iterFlow(rcNb)) = m_vDirection[iNb];
							vNbs.push_back(rcNb);
						}
						if(( *(iterFlowFlag(rcNb)) == m_ContFlat) &&
							  !(isEven(iNb)) &&
							  m_vFlowSelf[iNb] != *(iterFlow(*pos)))
						{
							*(iterFlow(rcNb)) = m_vDirection[iNb];
						}
						iNb++;
					}
				}
			}
			vOutlets.swap(vNbs);
		}while (vNbs.size() != 0);
}

void MapFlowDirection::InitPars()
{
		//	Location number				Order in m_vDirection
		//	-------								-------	 looping order of the neighbors
	  //	|6|7|8|								|0|1|2|
		//	-------								-------
		//	|5| |1|								|3|4|5|
		//	-------								-------
		//	|4|3|2|								|6|7|8|
		//	-------								-------
		//
		m_vDirection.resize(9);

		m_vDirection[0] = 2;
		m_vDirection[1] = 3;
		m_vDirection[2] = 4;
		m_vDirection[3] = 1;
		m_vDirection[4] = 0;
		m_vDirection[5] = 5;
		m_vDirection[6] = 8;
		m_vDirection[7] = 7;
		m_vDirection[8] = 6;

		m_vFlowSelf.resize(9);
		m_vFlowSelf[0] = 6;
		m_vFlowSelf[1] = 7;
		m_vFlowSelf[2] = 8;
		m_vFlowSelf[3] = 5;
		m_vFlowSelf[4] = 0;
		m_vFlowSelf[5] = 1;
		m_vFlowSelf[6] = 4;
		m_vFlowSelf[7] = 3;
		m_vFlowSelf[8] = 2;
}

FlowDirectionAlgorithm::Method FlowDirectionAlgorithm::methodValueOf(QString val) {
	if (val== QString("slope")) {
		return FlowDirectionAlgorithm::slope;
	}
	return FlowDirectionAlgorithm::height;
}

FlowDirectionAlgorithm::FlowDirectionAlgorithm(IRasterCoverage inRaster, IRasterCoverage flowdirRaster, IRasterCoverage emptyRaster)
	: _inRaster(inRaster)
	, _flowdirRaster(flowdirRaster)
	, _iterEmptyRaster(emptyRaster)
	, flatcell(9)
	, flag(10)
	, increment(1)
{
	//	Location number				Order in m_vDirection
	//	-------						-------	 looping order of the neighbors 	
	//	|6|7|8|						|0|1|2|
	//	-------						-------
	//	|5| |1|						|3|4|5|
	//	-------						-------
	//	|4|3|2|						|6|7|8|
	//	-------						-------
	//

	Location[0] = 6;
	Location[1] = 7;
	Location[2] = 8;
	Location[3] = 5;
	Location[4] = 1;
	Location[5] = 4;
	Location[6] = 3;
	Location[7] = 2;

	_xsize = _inRaster->size().xsize();
	_ysize = _inRaster->size().ysize();

}

bool FlowDirectionAlgorithm::onEdge(Pixel pix) {
	return pix.x == 0 || pix.x == _xsize - 1 ||
		pix.y == 0 || pix.y == _ysize - 1;
}

bool FlowDirectionAlgorithm::isEven(int elem)
{
	return elem % 2 == 0;
}


double MapFlowDirection::rFindMaxLocation(std::vector<double>& vValue, std::vector<int>& vPos, int& iCout)
{
	//finds the maximum value in the input vector
		//returns the maximum value, number of elements with max.
		//returns posision(s) for the element(s) with max. in a vector

	std::vector<double>::iterator pos;

	//returns the position of the first element with max. in vValue
	pos = max_element(vValue.begin(), vValue.end());
	double rMax = *pos;

	//count number of elements with max
	iCout = std::count(vValue.begin(), vValue.end(), rMax);

	//find the first element with max value
	pos = find(vValue.begin(), vValue.end(), rMax);
	int iIndex = pos - vValue.begin();

	while (pos != vValue.end())
	{
		vPos.push_back(iIndex);  //push it into a vector
		pos = find(++pos, vValue.end(), rMax);
		iIndex = pos - vValue.begin();
	}
	return rMax;
}

long MapFlowDirection::iLookUp(double rMax, int iCout, std::vector<int>& vPos)
{
	//rMax - max. value in slope / height diff.
	//iCout - number of positions with max. elements in vPos
	//vPos - positions with elements having max. value
	//iPos - location which the target cell flows to

	//This function examines the positons for elements with max value
	//then assigns flow direction accordinngly
	long iPos;
	if (rMax <= 0)
		iPos = 9; //a sink or flat cell
	else if (iCout < 3)
		iPos = vPos[0] + 1;
	else {
		//examine the positions of elements with max.
		//if three cells are adjacent, assign it to flow to the center
		//else flow to the first elem. in vector vPos for positions
		if (isInOneEdge(0, 1, 7, vPos))
			iPos = 1;
		else if (isInOneEdge(1, 2, 3, vPos))
			iPos = 3;
		else if (isInOneEdge(3, 4, 5, vPos))
			iPos = 5;
		else if (isInOneEdge(5, 6, 7, vPos))
			iPos = 7;
		else
			iPos = vPos[0] + 1;
	}
	return iPos;
}

int FlowDirectionAlgorithm::noflow = 0;

bool FlowDirectionAlgorithm::isInOneEdge(const std::vector<FlowDirection>& listPos, FlowDirection fd1, FlowDirection fd2, FlowDirection fd3) {

	bool fCondition1 = find(listPos.begin(), listPos.end(), fd1) != listPos.end();
	bool fCondition2 = find(listPos.begin(), listPos.end(), fd2) != listPos.end();
	bool fCondition3 = find(listPos.begin(), listPos.end(), fd3) != listPos.end();

	return fCondition1 && fCondition2 && fCondition3;
}

bool FlowDirectionAlgorithm::hasFlow(byte flowdirection) {
	if (flowdirection >= 1 && flowdirection <= 8)
		return true;
	else
		return false;
}


//Get the maximum value of the adjcent cells. if the elements of a list with the slope values with the eight neighboring cells
//Parameters
//		row - row number of the given cell
//		col - column number of the given cell
//		listVal - the list filled with the slop values against the eight neighboring cells	
//Returns
//		the maximum value     	
double FlowDirectionAlgorithm::maxAdj(Pixel pxl, double listVal[]) 
{
	int pos = 0;
	int index = 0;
	double val = 0, rheight, nheight;
	rheight = *(iterDEM(pxl));
	double max = -1;
	Pixel pospxl;

	for (int i = -1; i <= 1; ++i) 
	{
		for (int j = -1; j <= 1; ++j) 
		{
			pospxl.x = pxl.x + j;
			pospxl.y = pxl.y + i;
			pospxl.z = 0;
			pos = pos + 1;
			if (pos != 5) {
				if (*(iterDEM(pospxl)) == rUNDEF)
					return rUNDEF;
				nheight = *(iterDEM(pospxl));
				switch (method) 
				{
				case slope:
					val = computeSlope(rheight, nheight, pos);
					break;
				case height:
					val = computeHeightDifference(rheight, nheight);
					break;
				}
				listVal[index] = val;
				if (val > max)
					max = val;
				index++;
			}
		}
	}
	return max;
}

double FlowDirectionAlgorithm::maxAdj(Pixel pxl, PixelIterator gradient, double listVal[]) 
{
	int pos = 0;
	int index = 0;
	double val = 0, height, nheight;
	height = *(gradient(pxl));
	double max = -1;
	Pixel pospxl;

	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -1; j <= 1; ++j)
		{
			pospxl.x = pxl.x + j;
			pospxl.y = pxl.y + i;
			pospxl.z = 0;
			pos = pos + 1;
			if (pos != 5)
			{
				if ( *(iterDEM(pospxl)) > *(iterDEM(pxl)) )
					val = rUNDEF;
				else 
				{
					nheight = *(gradient(pospxl));
					val = computeHeightDifference(height, nheight);
				}
				listVal[index] = val;
				if (val > max)
					max = val;
				index++;
			}
		}
	}
	
	return max;
}


void FlowDirectionAlgorithm::calculate(QString methodInput) 
{
	QString sl = methodInput.toLower();
	method = methodValueOf(sl);

	FlowDirection fd;
	double max;

	iterFlow = PixelIterator(_flowdirRaster, BoundingBox(), PixelIterator::fXYZ);

	iterDEM = PixelIterator(_inRaster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator inEnd = iterDEM.end();

	iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);

	while (iterPos != inEnd)
	{
		Pixel pxl = iterPos.position();
		*(iterFlow(pxl)) = noflow;  // ini value
		//Get maximum slope for the specified cell with it's eight neighboring cells
		double listVal[8];
		//vector<double> listVal;// = new vector<double>(8);
		max = maxAdj(pxl, listVal);
		if (max > 0)
		{
			//Finds thw elements with maximum value in the list
			std::vector<FlowDirection> listPos;
			findDirection(listVal, max, listPos);
			fd = getFlowDirection(listPos);
			*(iterFlow(pxl)) = Location[(byte)fd];
		}
		else if (max == rUNDEF)
			*(iterFlow(pxl)) = noflow;
		else
			*(iterFlow(pxl)) = flatcell;
		iterPos++;
	}
	//-------------------------------------------------------------------------
	int copylist = itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF;
	IRasterCoverage gradient1Raster = OperationHelperRaster::initialize(_inRaster.as<IlwisObject>(), itRASTER, copylist);
	if (!gradient1Raster.isValid())
	{
		ERROR1(ERR_NO_INITIALIZED_1, "gradient1");
		return;
	}

	IRasterCoverage gradient2Raster = OperationHelperRaster::initialize(_inRaster.as<IlwisObject>(), itRASTER, copylist);
	if (!gradient2Raster.isValid())
	{
		ERROR1(ERR_NO_INITIALIZED_1, "gradient2");
		return;
	}

	PixelIterator gradient1 = PixelIterator(gradient1Raster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator gradient2 = PixelIterator(gradient2Raster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator grdEnd = gradient1.end();
	//-----------------------------------------------------------------------------------------

	//Flat surface treatment
	std::fill(gradient1, gradient1.end(), 0);
	std::fill(gradient2, gradient2.end(), 0);
	
	iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);

	while (iterPos != grdEnd)
	{
		Pixel pxl = iterPos.position();
		if (*(iterFlow(pxl)) == flatcell)
		{
			std::vector<Cell> flatList;
			std::vector<Cell> outletList;
			locateOutlet(pxl, flatList, outletList);
			if (outletList.size() > 0) 
			{
				imposeGradient2LowerElevation(outletList, flatList, gradient1);
				imposeGradientFromElevation(flatList, gradient2);
				combineGradient(gradient1, gradient2, flatList);
				assignFlowInFlat(flatList, gradient2);
				Cell cell;
				for (std::vector<Cell>::iterator pos = outletList.begin();
					pos < outletList.end(); ++pos)
				{
					cell = *pos;
					*(iterFlow(cell.pxl)) = cell.val;
				}

				//Apply flow assignment procedure to those exceptional cells based on gradient1
				assignFlowInFlat(flatList, gradient1);
				iniGradient(gradient1, gradient2, flatList);
			}
		}
		iterPos++;
	}

	iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);
	while (iterPos != grdEnd)
	{
		Pixel pxl = iterPos.position();
		if (onEdge(pxl)  || *(iterFlow(pxl)) > 8)
			*(iterFlow(pxl)) = 0;
		iterPos++;
	}
}

//The slope is calculated by subscribing the neighbor's elevation value from the center
//distance 1.14 is concerned for diagonal cells
//Parameters
//h1 - the elevation of the center cell
//h2 - the elevation of the neighboring cell
//pos - the location number according to flow direction definition in ILWIS 	
//Returns - slope value of the center cell	
double FlowDirectionAlgorithm::computeSlope(double h1, double h2, int pos) 
{
	double val;
	if (isEven(pos))
		val = h1 - h2;
	else
		val = (h1 - h2) / 1.41;
	return val;
}


double FlowDirectionAlgorithm::computeHeightDifference(double h1, double h2) 
{
	return h1 - h2;
}


//Finds the elements with the specified value in the given list
//Parameters
//	listA - the list filled with the slopes with the neighboring cells
//	val - the specified value to be searched in the given list
//  Returns - the list filled with the index of the specified element 	
void FlowDirectionAlgorithm::findDirection(double listA[], double val,
	std::vector<FlowDirectionAlgorithm::FlowDirection>& listPos) 
{
	for (int index = 0; index < 8; ++index) {
		double value = listA[index];
		if (value == val)
			listPos.push_back(mapFlowLocation(index));
	}
}

//	Map the flow location number with flow direction
//	Parameters
//		pos - the flow location number as defined in ILWIS
//	Returns - flow direction 	
FlowDirectionAlgorithm::FlowDirection FlowDirectionAlgorithm::mapFlowLocation(int pos) 
{
	FlowDirection result = FlowDirectionAlgorithm::E;
	switch (pos) {
	case 0: result = FlowDirectionAlgorithm::NW;
		break;
	case 1: result = FlowDirectionAlgorithm::N;
		break;
	case 2:	result = FlowDirectionAlgorithm::NE;
		break;
	case 3: result = FlowDirectionAlgorithm::W;
		break;
	case 4: result = FlowDirectionAlgorithm::E;
		break;
	case 5: result = FlowDirectionAlgorithm::SW;
		break;
	case 6: result = FlowDirectionAlgorithm::S;
		break;
	case 7: result = FlowDirectionAlgorithm::SE;
		break;
	}
	return result;
}


//Examine the flow direction location to perform one of the following:
//---1. if there are only two elements in the given list (maxium slop occurs in two neiboring cells):	
//---2. if there are only two elements in the given list (maxium slop occurs in two neiboring cells):
//make flow direction with S or W or E or N, if such a element exists in the given list
//otherwise, make the flow direction with the first element in the list.
//---3. if there are more than two cells with maximum slop, perform one of the following:
//if three cells located in one edge, make the flow direction value with the middle cell of the edge, otherwise,
//make flow direction with S or W or E or N, if such a element exists in the given list, otherwise
//make the flow direction with the first element in the list 	
//Parameters
//	listPos - the list filled with flow directions
//Returns - flow direction 	
FlowDirectionAlgorithm::FlowDirection FlowDirectionAlgorithm::getFlowDirection(const std::vector<FlowDirection>& listPos) 
{
	//vector<FlowDirection>::iterator pos = listPos.iterator();
	if (listPos.size() == 1) {
		return listPos[0];
	}
	else if (listPos.size() == 2) { //sets it flow witho the center element, if it exists
		int index = 0;
		while (index < 2) {
			FlowDirection fd = listPos[index];
			if ((fd == FlowDirectionAlgorithm::E) || (fd == FlowDirectionAlgorithm::S) ||
				(fd == FlowDirectionAlgorithm::W) || (fd == FlowDirectionAlgorithm::N)) {
				return fd;
			}
			index++;
		}
	}
	else {
		if (isInOneEdge(listPos, FlowDirectionAlgorithm::SW, FlowDirectionAlgorithm::S, FlowDirectionAlgorithm::SE))
			return FlowDirectionAlgorithm::S;
		else if (isInOneEdge(listPos, FlowDirectionAlgorithm::NW, FlowDirectionAlgorithm::W, FlowDirectionAlgorithm::SW))
			return FlowDirectionAlgorithm::W;
		else if (isInOneEdge(listPos, FlowDirectionAlgorithm::NW, FlowDirectionAlgorithm::N, FlowDirectionAlgorithm::NE))
			return FlowDirectionAlgorithm::N;
		else if (isInOneEdge(listPos, FlowDirectionAlgorithm::NE, FlowDirectionAlgorithm::E, FlowDirectionAlgorithm::SE))
			return FlowDirectionAlgorithm::E;
		else {
			int index = 0;
			while (index < listPos.size()) {
				FlowDirection fd = listPos[index];
				if (fd == FlowDirectionAlgorithm::E || fd == FlowDirectionAlgorithm::S ||
					fd == FlowDirectionAlgorithm::W || fd == FlowDirectionAlgorithm::N) {
					return fd;
				}
				index++;
			}
		}
	}
	return listPos[0];
}

//	Locates cells in the flat adjacent to a outlet and add this cell to a list
//	Parameters
//		row - row number of the given cell in a flat
//		col - column number of the given cell in a flat
//		flatList - the list filled with the cells over a flat
//	Returns - a list filled with cells over the flat adjacent to an outlet cell 	
void FlowDirectionAlgorithm::locateOutlet(Pixel pxl, std::vector<Cell>& flatList, std::vector<Cell>& outList) {
	int nrow, ncol;
	Cell cell(pxl);
	std::vector<Cell> srcList;
	std::vector<Cell> desList;
	*(iterFlow(pxl)) = flag;
	double flatCellElevation = *(iterDEM(pxl));
	srcList.push_back(cell);
	flatList.push_back(cell);

	do {
		desList.resize(0);
		for (std::vector<Cell>::iterator pos = srcList.begin(); pos < srcList.end(); ++pos)
		{
			cell = *pos;
			pxl.y = cell.pxl.y;
			pxl.x = cell.pxl.x;
			Pixel pospxl;
			for (int i = -1; i <= 1; ++i) 
			{
				pospxl.y = pxl.y + i;
				for (int j = -1; j <= 1; ++j)
				{
					pospxl.x = pxl.x + j;
					pospxl.z = 0;

					if (!onEdge(pospxl) && *(iterFlow(pospxl)) != flag )
					{
						if (*(iterDEM(pospxl)) == flatCellElevation && hasFlow( *(iterFlow(pospxl))) ) 
						{
							cell.setRC(pospxl);
							cell.val = *(iterFlow(pospxl));
							*(iterFlow(pospxl)) = flag;
							outList.push_back(cell);
							desList.push_back(cell);
						}
						else if (*(iterDEM(pospxl)) == flatCellElevation) 
						{
							cell.setRC(pospxl);
							*(iterFlow(pospxl)) = flag;
							desList.push_back(cell);
							flatList.push_back(cell);
						}
					}
				}
			}
		}
		srcList.swap(desList);
	} while (srcList.size() > 0);
}

//	Impose gradient in flat to lower elevation. This incrementation starts with adjacent cells to existing gradient,   
//	by imposing a downslope gradient. The procedure then is re-applied to all the remainder cells.
//	This is repeated untill all cells have a downstream gradient. 
//	Parameters
//		flow - an array with flow direction
//		outletList - the list filled with the cells adjacent to the outlets
//		flatList - the list filled with the cells in a flat surface	
//	Returns - an array filled with gradient to lower elevation in a flat 	
void FlowDirectionAlgorithm::imposeGradient2LowerElevation(std::vector<Cell>& outletList, std::vector<Cell>& flatList, PixelIterator gradient) 
{
	Pixel pxl;
	pxl.x = 0;
	pxl.y = 0;
	pxl.z = 0;

	Cell cell(pxl);

	std::vector<Cell> srcList;
	for (std::vector<Cell>::iterator pos = outletList.begin(); pos < outletList.end(); ++pos) 
	{
		srcList.push_back((*pos));
	}
	std::vector<Cell> desList;
	do {
		for (std::vector<Cell>::iterator pos = srcList.begin(); pos < srcList.end(); ++pos) 
		{
			cell = (*pos);
			*(iterFlow(cell.pxl))= flatcell;
		}

		for (std::vector<Cell>::iterator pos = flatList.begin(); pos < flatList.end(); ++pos) 
		{
			//int index = li.nextIndex();
			cell = (*pos);
			if (*(iterFlow(cell.pxl)) == flag) 
			{
				*(gradient(cell.pxl)) += increment;
			}
		}
		for (std::vector<Cell>::iterator pos = srcList.begin(); pos < srcList.end(); ++pos)
		{
			cell = (*pos);
			pxl.y = cell.pxl.y;
			pxl.x = cell.pxl.x;
			Pixel pospxl;
			pospxl.z = 0;
			for (int i = -1; i <= 1; ++i) 
			{
				pospxl.y = pxl.y + i;
				for (int j = -1; j <= 1; ++j) {
					pospxl.x = pxl.x + j;
					if (*(iterFlow(pospxl)) == flag) {
						cell.setRC(pospxl);
						desList.push_back(cell);
						*(iterFlow(pospxl)) = flatcell;
					}
				}
			}
		}
		srcList.swap(desList);
		desList.resize(0);
	} while (srcList.size() > 0);
	srcList.resize(0);
}


//	Impose gradient away from higher elevation. 
//	Parameters
//		flow - an array with flow direction
//		outletList - the list filled with the cells adjacent to the outlets
//	Returns - an array filled with gradient to lower elevation in a flat 	
void FlowDirectionAlgorithm::imposeGradientFromElevation(std::vector<Cell>& flatList, PixelIterator gradient)
{
	Pixel pxl;
	pxl.x = 0;
	pxl.y = 0;
	pxl.z = 0;

	Cell cell(pxl);

	bool condition1 = false, condition2 = false;
	do {
		condition2 = false;
		std::vector<Cell> flagList;
		for (std::vector<Cell>::iterator pos = flatList.begin(); pos < flatList.end(); ++pos)
		{
			cell = (*pos);
			pxl.y = cell.pxl.y;
			pxl.x = cell.pxl.x;
			pxl.z = 0;
			Pixel pospxl;
			pospxl.z = 0;

			if (*(iterFlow(pxl)) != flag) {
				condition1 = true;
				condition2 = true;
				for (int i = -1; i <= 1; ++i) 
				{
					pospxl.y = pxl.y + i;
					for (int j = -1; j <= 1; ++j)
					{
						pospxl.x = pxl.x + j;
						if (*(iterFlow(pospxl)) != flatcell || *(iterFlow(pospxl)) == flag ) 
						{
							flagList.push_back(cell);
							condition1 = false;
							break;
						}
					}
					if (condition1 == false)
						break;
				}
			}
		}

		for ( std::vector<Cell>::iterator pos = flagList.begin();
			pos < flagList.end(); ++pos) 
		{
			cell = (*pos);
			*(iterFlow(cell.pxl)) = flag;
		}

		if (flagList.size() > 0) 
		{
			for (std::vector<Cell>::iterator pos = flatList.begin();
				pos < flatList.end(); ++pos) 
			{
				cell = (*pos);
				if (*(iterFlow(cell.pxl)) == flag)
				{
					*(gradient(cell.pxl)) += increment;
				}
			}
		}
		condition2 = flagList.size() > 0;
	} while (condition2);
	//return gradient;
}

//	Combine gradient in the previous two steps, 
//	imposeGradient2LowerElevation and imposeGradientFromElevation
//	Parameters
//		grds - an array resulting from the previous step - imposeGradientFromElevation
//		flatList - the list filled with the cells in a flat surface 
void FlowDirectionAlgorithm::combineGradient( PixelIterator grd1, PixelIterator grd2, std::vector<Cell>& flatList) 
{
	Cell cell;
	for (std::vector<Cell>::iterator pos = flatList.begin();pos < flatList.end(); ++pos) 
	{
		cell = (*pos);
		*(grd2(cell.pxl)) += *(grd1(cell.pxl));
	}
}

void assignFlowInFlat(std::vector<Cell>& flatList, PixelIterator gradient);
/*Assign flow direction based on D-8 approach using the result from the previous step combineGradient
* @param flatList - the list filled with the cells in a flat surface
*/
void FlowDirectionAlgorithm::assignFlowInFlat(std::vector<Cell>& flatList, PixelIterator gradient) 
{
	Cell cell;
	for (std::vector<Cell>::iterator pos = flatList.begin();pos < flatList.end(); ++pos) 
	{
		cell = (*pos);
		
		if (hasFlow(*(iterFlow(cell.pxl))) != true) {
			double listVal[8];
			double max = maxAdj(cell.pxl, gradient, listVal);
			if (max > 0) {
				std::vector<FlowDirection> listPos;
				findDirection(listVal, max, listPos);
				FlowDirection fd = getFlowDirection(listPos);
				*(iterFlow(cell.pxl)) = Location[(byte)fd];
			}
		}
	}
}


void FlowDirectionAlgorithm::iniGradient(PixelIterator grd1, PixelIterator grd2, std::vector<Cell>& flatList) 
{
	Cell cell;
	for (std::vector<Cell>::iterator pos = flatList.begin();pos < flatList.end(); ++pos) 
	{
		cell = (*pos);
		*(grd2(cell.pxl)) = 0;
		*(grd1(cell.pxl)) = 0;		
	}
}
