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
								    int lineSize)
{
	// calc dirBoundsCurrent with horizontal directions by comparing vertical differences of pixel rows
		// if upper pixel > lower pixel then direction is RIGHT
	for (int j = -1; j <= lineSize; ++j) {
		int x = j + 1;
		if (inputLinePrev[x] == inputLine[x])
			_dirBoundsCurrent[x] = dbNONE;
		else if (inputLinePrev[x] > inputLine[x])
			_dirBoundsCurrent[x] = dbRIGHT;
		else
			_dirBoundsCurrent[x] = dbLEFT;
	}
	// calc dirBoundsNext with vertical directions by comparing horizontal differences of pixel columns
		// if left pixel > right pixel then direction is UP

	for (int j = 0; j <= lineSize; ++j) {
		int x = j + 1;
		if (inputLine[x - 1] == inputLine[x])
			_dirBoundsNext[x] = dbNONE;
		else if (inputLine[x - 1] > inputLine[x])
			_dirBoundsNext[x] = dbUP;
		else
			_dirBoundsNext[x] = dbDOWN;
	}
}

byte RasterToPolygon::setPixelFlag(int x)  {
	byte b = 0;
	if (_dirBoundsCurrent[x] != dbNONE)
		b |= 1;
	if (_dirBoundsPrev[x] != dbNONE)
		b |= 2;
	if (_dirBoundsCurrent[x - 1] != dbNONE)
		b |= 4;
	if (_dirBoundsNext[x] != dbNONE)
		b |= 8;

	return b;
}

void RasterToPolygon::handleNodeCases(int x, int y, byte pixelFlag, const std::vector<double>& inputLine, const std::vector<double>& inputLinePrev)  {
	switch (pixelFlag)
	{
		case 0:
		{
			_segBoundsHoriz[0] = 0;
			_segBoundsVert[0] = 0;
		}
		break;
		case 7: case 11: case 13: case 14:
		{
			newNode(x, y, pixelFlag, inputLine, inputLinePrev);
		}
		break;
		case 15:
		{
			if (!_eightConnected)
				newNode(x, y, pixelFlag, inputLine, inputLinePrev);
			else
			{
				bool f1 = inputLinePrev[x] == inputLine[x - 1];
				bool f2 = inputLinePrev[x - 1] == inputLine[x];
				if (f1 && f2)
				{
					if (inputLine[x - 1] > inputLine[x])
						f2 = false;
					else
						f1 = false;
				}
				if (f1)
				{
					appendLeftUp(x, y); // b = 6
					//b = 9
					_segBoundsVert[x] = _segBoundsHoriz[x] = newInBetween(x);

				}
				else if (f2)
				{
					//->appendUp(x,y,1);
					auto h = _segBoundsHoriz[x];
					//->appendLeft(x,y,0/*12*/);
					_segBoundsHoriz[x] = h;
				}
				else
					newNode(x, y, pixelFlag, inputLine, inputLinePrev);
			}
			break;
		}
	}
}

void RasterToPolygon::appendLeftUp(int x, int y)
/* Appends codes of point (iLine, iCol) to segment to the left of point.
   The segment above the point is chained to the left segment.
   Is called if type of point = 6 (line from left to up or line
   from up to left.                                              */
{
	enum WhatDel { DELNONE, DELLEFT, DELUP } del;
	auto sbUp = _segBoundsVert[x];
	auto sbLeft = _segBoundsHoriz[x - 1];
	if (sbLeft != sbUp) {
		del = DELNONE;
		if (_dirBoundsCurrent[x - 1] == dbLEFT) { //line comes from above, goes left }
			if (sbUp->fBeginSeg) {
				sbUp->dlChain.insert(sbUp->dlChain.end(), sbLeft->dlChain.begin(), sbLeft->dlChain.end());
				//        for (DLIter<ChainRec> iter(&sbLeft->dlChain); iter.fValid(); iter++) 
				//          sbUp->dlChain.append(iter());
				del = DELLEFT;
				sbUp->fEndSeg = sbLeft->fEndSeg;
				if (sbUp->fEndSeg) {
					_fwl[sbUp->iSegNr] = _fwl[sbLeft->iSegNr];
					for (unsigned long i = 1; i <= _bwl.size(); ++i) {
						if (_bwl[i] == -sbLeft->iSegNr)
							_bwl[i] = -sbUp->iSegNr;
						else if (_bwl[i] == sbLeft->iSegNr)
							_bwl[i] = sbUp->iSegNr;
						else if (_fwl[i] == -sbLeft->iSegNr)
							_bwl[i] = -sbUp->iSegNr;
						else if (_fwl[i] == sbLeft->iSegNr)
							_bwl[i] = sbUp->iSegNr;
					}
				}
				sbUp->crdTo = sbLeft->crdTo;
				StoreSegm(*sbUp);
			}
			else { // sbUp->fBeginSeg == false
				//sbLeft->dlChain.insert(sbUp->dlChain);
				sbLeft->dlChain.insert(sbLeft->dlChain.begin(), sbUp->dlChain.begin(), sbUp->dlChain.end());
				//        DLIter<ChainRec> iter(&sbUp->dlChain);
				//        for (iter.last(); iter.fValid(); iter--) 
				//          sbLeft->dlChain.insert(iter());
				del = DELUP;
			}
		}
		else { // line comes from left, goes to above 
			if (sbUp->fEndSeg) {
				//sbUp->dlChain.insert(sbLeft->dlChain);
				sbUp->dlChain.insert(sbUp->dlChain.begin(), sbLeft->dlChain.begin(), sbLeft->dlChain.end());

				//          DLIter<ChainRec> iter(&sbLeft->dlChain);
				//          for (iter.last(); iter.fValid(); iter--)
				//            sbUp->dlChain.insert(iter());
				del = DELLEFT;
				sbUp->fBeginSeg = sbLeft->fBeginSeg;
				if (sbUp->fBeginSeg) {
					_bwl[sbUp->iSegNr] = _bwl[sbLeft->iSegNr];
					for (unsigned long i = 1; i <= _fwl.size(); ++i) {
						if (_fwl[i] == sbLeft->iSegNr)
							_fwl[i] = sbUp->iSegNr;
						else if (_fwl[i] == -sbLeft->iSegNr)
							_fwl[i] = -sbUp->iSegNr;
						else if (_bwl[i] == sbLeft->iSegNr)
							_bwl[i] = sbUp->iSegNr;
						else if (_bwl[i] == -sbLeft->iSegNr)
							_bwl[i] = -sbUp->iSegNr;
					}
				}
				sbUp->crdFrom = sbLeft->crdFrom;
				StoreSegm(*sbUp);
			}
			else {
				//sbLeft->dlChain.append(sbUp->dlChain);
				sbLeft->dlChain.insert(sbLeft->dlChain.end(), sbUp->dlChain.begin(), sbUp->dlChain.end());
				//        for (DLIter<ChainRec> iter(&sbUp->dlChain); iter.fValid(); iter++) 
			  //            sbLeft->dlChain.append(iter());
				del = DELUP;
			}
		}
		// adjust sbHoriz and sbVert by replacing the deleted SegBound with the other one
		if (del == DELLEFT) {
			for (long i = 0; i <= _segBoundsHoriz.size(); i++) {
				if (_segBoundsHoriz[i] == sbLeft)
					_segBoundsHoriz[i] = sbUp;
				if (_segBoundsVert[i] == sbLeft)
					_segBoundsVert[i] = sbUp;
			}
			_segNr.push_back(sbLeft->iSegNr);
			_fwl[sbLeft->iSegNr] = 0;
			_bwl[sbLeft->iSegNr] = 0;
		}
		else if (del == DELUP) {
			for (long i = 0; i <= _segBoundsHoriz.size(); i++) {
				if (_segBoundsHoriz[i] == sbUp)
					_segBoundsHoriz[i] = sbLeft;
				if (_segBoundsVert[i] == sbUp)
					_segBoundsVert[i] = sbLeft;
			}
			_segNr.push_back(sbUp->iSegNr);
			_fwl[sbUp->iSegNr] = 0;
			_bwl[sbUp->iSegNr] = 0;
		}
		else {
			StoreSegm(*sbLeft);
			StoreSegm(*sbUp);
		}
	}
	else { // segment is contour of island 
	  // create new node; start and end of segment 
	  // modify segment 
		sbUp->fBeginSeg = true;
		sbUp->crdFrom.x = x; 
		sbUp->crdFrom.y = y;
		sbUp->fEndSeg = true;
		sbUp->crdTo.x = x; 
		sbUp->crdTo.y = y;
		_fwl[sbUp->iSegNr] = sbUp->iSegNr;
		_bwl[sbUp->iSegNr] = -sbUp->iSegNr;
		StoreSegm(*sbUp);
	}
	_segBoundsHoriz[x] = 0;
	_segBoundsVert[x] = 0;
}

void RasterToPolygon::StoreSegm(SegBoundPtr seg) {

}

RasterToPolygon::SegBoundPtr RasterToPolygon::newInBetween(int x) {

}
void RasterToPolygon::newNode(int x, int y, byte pixelFlag, const std::vector<double>& inputLine, const std::vector<double>& inputLinePrev)  {
	std::vector<bool> segExist(dbRIGHT + 1);
	std::vector<bool> beginSeg(dbRIGHT + 1);
	std::vector<SegBoundPtr> sbSeg(dbRIGHT + 1);

	segExist[dbRIGHT] = pixelFlag & 1;
	segExist[dbUP] = (pixelFlag & 2) != 0;
	segExist[dbLEFT] = (pixelFlag & 4) != 0;
	segExist[dbDOWN] = (pixelFlag & 8) != 0;

	if (segExist[dbRIGHT]) {// new segment to the right 
		sbSeg[dbRIGHT] = newWithOneEnd(x, y, inputLine, inputLinePrev, true, (bool &)beginSeg[dbRIGHT]);
		_segBoundsHoriz[x] = sbSeg[dbRIGHT];
	}

}

RasterToPolygon::SegBoundPtr RasterToPolygon::newWithOneEnd(int x, int y, const std::vector<double>& inputLine, const std::vector<double>& inputLinePrev, bool isRight, bool& isBegin) {
	// Creates new segment with a node at one end.                 
	// If fRightSeg==true : it has to be a segment to the right of node,
	// else a segment under the node.                                 

	DirBound db;
	if (isRight)
		db = (DirBound)_dirBoundsCurrent[x];
	else
		db = (DirBound)_dirBoundsNext[x];
	SegBoundPtr sb;
	sb.reset(new SegBound());

	sb->iSegNr = newSegNr();
	if (sb->iSegNr >= (int)_fwl.size()) {
		_fwl.resize(sb->iSegNr + 1);
		_bwl.resize(sb->iSegNr + 1);
	}

	_fwl[sb->iSegNr] = _bwl[sb->iSegNr] = 0;
	if ((db == dbRIGHT) || (db == dbDOWN)) { // start of segment 
		sb->Insert(db, 1); // at start
		sb->fBeginSeg = true;
		isBegin = true;
		sb->fEndSeg = false;
		sb->crdFrom.x = x; 
		sb->crdFrom.y = y;
		sb->crdTo.x = sb->crdTo.y = 0;
	}
	else { // end of segment 
		sb->Append(db, 1); // at end
		sb->fBeginSeg = false;
		isBegin = false;
		sb->fEndSeg = true;
		sb->crdFrom.x = sb->crdFrom.y = 0;
		sb->crdTo.x = x; 
		sb->crdTo.y = y;
	}

	switch (db) {
	case dbRIGHT: {
		//            top.PolL := last_line^[x]; top.PolR := line^[x];
		sb->iLeftRaw = inputLinePrev[x];
		sb->iRightRaw = inputLine[x];
	}
				  break;
	case dbUP: {
		//              top.PolL := line^[x - 1]; top.PolR := line^[x];
		sb->iLeftRaw = inputLine[x - 1];
		sb->iRightRaw = inputLine[x];
	}
			   break;
	case dbLEFT: {
		//              top.PolL := line^[x]; top.PolR := last_line^[x];
		sb->iLeftRaw = inputLine[x];
		sb->iRightRaw = inputLinePrev[x];
	}
				 break;
	case dbDOWN: {
		//                top.PolL := line^[x]; top.PolR := line^[x - 1];
		sb->iLeftRaw = inputLine[x];
		sb->iRightRaw = inputLine[x - 1];
	}
	break;
	default: 
		break;
	}
	return sb;
}

quint32 RasterToPolygon::newSegNr()
{
	long ind = _segNr.size() - 1;
	if (ind >= 0) {
		long segnr = _segNr[ind];
		_segNr.erase(_segNr.begin() + ind);
		return segnr;
	}
	return ++_nrSeg;
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
	_dirBoundsNext.resize(inputLine.size(), dbNONE);
	_dirBoundsPrev.resize(inputLine.size(), dbNONE);
	_segBoundsHoriz.resize(inputLine.size());
	_segBoundsVert.resize(inputLine.size());
	_dirBoundsCurrent.resize(inputLine.size(), dbNONE);

	inputLine.front() = inputLine.back() = rUNDEF;

	PixelIterator iter(_inputraster);
	PixelIterator iterEnd = iter.end();
	while (iter != iterEnd) {
		inputLine[iter.x() + 1] = *iter;
		if (iter.ychanged()) {
			fillLineInfo(inputLine, inputLinePrev, lineSize);

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
				byte pixelFlag = setPixelFlag(x);
				handleNodeCases(x,iter.y(),  pixelFlag, inputLine, inputLinePrev);
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

