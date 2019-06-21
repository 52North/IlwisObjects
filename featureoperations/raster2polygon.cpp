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
					_segBoundsVert[x] = _segBoundsHoriz[x] = newInBetween(x, inputLine, inputLinePrev);

				}
				else if (f2)
				{
					appendUp(x,1);
					auto h = _segBoundsHoriz[x];
					appendLeft(x,0/*12*/);
					_segBoundsHoriz[x] = h;
				}
				else
					newNode(x, y, pixelFlag, inputLine, inputLinePrev);
			}
			break;
		}
		case 3: case 10: // append to upper segment
		{
			appendUp(x, pixelFlag);
		}
		break;
		case 5: case 12: // append to left segment
		{
			appendLeft(x, pixelFlag);
		}
		break;
		case 6: //  append to left and up (become same segment)
		{
			appendLeftUp(x,y);
		}
		break;
		case 9:
		{
			_segBoundsVert[x] = _segBoundsHoriz[x] = newInBetween(x, inputLine, inputLinePrev);
		}
		break;
		case 1: case 2: case 4: case 8: // does not occur
			break;
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
				storeSegm(sbUp);
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
				storeSegm(sbUp);
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
			storeSegm(sbLeft);
			storeSegm(sbUp);
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
		storeSegm(sbUp);
	}
	_segBoundsHoriz[x] = 0;
	_segBoundsVert[x] = 0;
}

void RasterToPolygon::appendUp(int x, byte pixelFlag)
// Append chain codes of (iLine,iRow) to segment above point
{
	SegBoundPtr sb = _segBoundsVert[x];
	DirBound db;
	if ((_dirBoundsCurrent[x] == dbLEFT) || ((pixelFlag != 1)) && (_dirBoundsNext[x] == dbUP)) {
		// line goes up 
		if (pixelFlag & 1)
			db = dbLEFT;
		else
			db = dbUP;
		if (sb->front().dbCode == db)
			sb->front().iLength++;
		else
			sb->Insert(db, 1); // insert at begin
	}
	else { // line goes down 
		if (pixelFlag & 1)
			db = dbRIGHT;
		else
			db = dbDOWN;
		if (sb->dlChain.back().dbCode == db)
			sb->back().iLength++;
		else
			sb->Append(db, 1); // append at end
	}
	if (pixelFlag == 10) {
		_segBoundsHoriz[x] = 0;
		_segBoundsVert[x] = sb;
	}
	else {
		_segBoundsVert[x] = 0;
		_segBoundsHoriz[x] = sb;
	}
	storeSegm(sb);
}

void RasterToPolygon::appendLeft(int x, byte pixelFlag)
// Append chain code from (iLine, iCol) to segment to the left.
{
	SegBoundPtr sb = _segBoundsHoriz[x - 1];
	DirBound db;
	if (((_dirBoundsCurrent[x] == dbLEFT) && (pixelFlag != 0)) || (_dirBoundsNext[x] == dbUP)) {
		// line goes left 
		if (pixelFlag & 1)
			db = dbLEFT;
		else
			db = dbUP;
		if (sb->front().dbCode == db)
			sb->front().iLength++;
		else
			sb->Insert(db, 1); // insert at begin
	}
	else { // line goes right 
		if (pixelFlag & 1)
			db = dbRIGHT;
		else
			db = dbDOWN;
		if (sb->dlChain.back().dbCode == db)
			sb->back().iLength++;
		else
			sb->Append(db, 1); // append at end
	}
	if (pixelFlag == 5) {
		_segBoundsVert[x] = 0;
		_segBoundsHoriz[x] = sb;
	}
	else {
		_segBoundsHoriz[x] = 0;
		_segBoundsVert[x] = sb;
	}
	storeSegm(sb);
}

void RasterToPolygon::storeSegm(const SegBoundPtr sb, std::vector<Coordinate>& coords, bool fIsland, long& crdIndex) {
/*	if (iCrd == 0)
		return;

	trq.fAborted();
	CoordBuf cBufTemp(iCrd);
	for (long j = 0; j < iCrd; ++j)
		map->gr()->RowCol2Coord(cBuf[j].y, cBuf[j].x, cBufTemp[j]);

	//geos::operation::polygonize::Polygonizer polygonizer;
   // ILWIS::Polygon *pol;
   // if ( sb.iSegNr > pms->iPol())
	  //pol = pms->newFeature();
   // else
	  //pol = pms->pol(sb.iSegNr);
   // if ( !pol)
	  //  throw ErrorObject(String("Couldn't find polygon nr %d",sb.iSegNr));
   // if(fIsland) {
	  //  pol->addHole(new LinearRing(cBufTemp.clone(), new GeometryFactory()));
   // }
   // else{
	  // pol->addBoundary(new LinearRing(cBufTemp.clone(), new GeometryFactory()));
	  // DomainSort* pdsrt = dm()->pdsrt();
	  // String sVal("%S | %S", map->dm()->sValueByRaw(sb.iLeftRaw,0), map->dm()->sValueByRaw(sb.iRightRaw,0));
	  // long iRaw = pdsrt->iAdd(sVal);
	  // pol->PutVal(iRaw);
   // }
	GeometryFactory *fact = new GeometryFactory();
	polygonizer.add((Geometry *)(fact->createLineString(cBufTemp.clone())));
	for (long j = 0; j < iCrd; ++j) {
		cBuf[j] = cBufTemp[j];
		TRACE(String("x=%f, y=%f ", cBufTemp[j].x, cBufTemp[j].y).scVal());
	}
	iCrd = 0;*/
}

void RasterToPolygon::storeSegm(SegBoundPtr seg) {
	if (!seg->fBeginSeg || !seg->fEndSeg)
		return;
	std::vector<Coordinate> coords;
	long iCrd = 0;
	bool isIland = false; // sb.crdFrom == sb.crdTo;
	if (_smooth) {

		// calculate coordinates
		Coordinate crdFrom = seg->crdFrom;
		Coordinate crdTo = seg->crdTo;
		std::vector<ChainRec> acr;
		std::copy(seg->dlChain.begin(), seg->dlChain.end(), std::back_inserter(acr));
		coords.resize(acr.size() * 2); // should be sufficient
		double x, y, dx, dy;
		x = crdFrom.x;
		y = crdFrom.y;
		long iPrevStep, iCurrStep, iNextStep;
		if (isIland) {
			ChainRec crLast = acr[acr.size() - 1];
			iPrevStep = crLast.iLength;
			if ((crLast.dbCode == dbUP) || (crLast.dbCode == dbLEFT))
				iPrevStep = -iPrevStep;
		}
		else {
			iPrevStep = 0;
			coords[iCrd++] = crdFrom;
		}
		iCurrStep = acr[0].iLength;
		if ((acr[0].dbCode == dbUP) || (acr[0].dbCode == dbLEFT))
			iCurrStep = -iCurrStep;
		Coordinate cFirst = Coordinate(crdFrom.x, crdFrom.y);
		for (unsigned long i = 0; i < acr.size(); ++i) {
			if (i != acr.size() - 1) {
				iNextStep = acr[i + 1].iLength;
				if ((acr[i + 1].dbCode == dbUP) || (acr[i + 1].dbCode == dbLEFT))
					iNextStep = -iNextStep;
			}
			else {
				if (isIland) {
					iNextStep = acr[0].iLength;
					if ((acr[0].dbCode == dbUP) || (acr[0].dbCode == dbLEFT))
						iNextStep = -iNextStep;
				}
				else
					iNextStep = 0;
			}
			switch (acr[i].dbCode)
			{
			case dbRIGHT: case dbLEFT: {
				dx = iCurrStep; dy = 0;
			}
						  break;
			case dbUP: case dbDOWN: {
				dx = 0; dy = iCurrStep;
			}
					   break;

			default:
				assert(0 == 1);
				break;
			}
			long iNewCrd = 0;
			if (abs(iCurrStep) == 1) {
				if (iPrevStep != iNextStep) {
					coords[iCrd++] = Coordinate(x + dx / 2, y + dy / 2);
					iNewCrd++;
				}
				if (!isIland && (iNextStep == 0)) {
					coords[iCrd++] = Coordinate(x + dx, y + dy);
					iNewCrd++;
				}
			}
			else {
				if ((abs(iPrevStep) == 1) || (iNextStep == 1)) {
					coords[iCrd++] = Coordinate(x + dx / 2, y + dy / 2);
					iNewCrd++;
				}
				if (abs(iNextStep) != 1) {
					coords[iCrd++] = Coordinate(x + dx, y + dy);
					iNewCrd++;
				}
			}
			if (isIland && (iCrd == iNewCrd))
				cFirst = coords[0];
			x += dx;
			y += dy;
			iPrevStep = iCurrStep;
			iCurrStep = iNextStep;
		}
		if (isIland) {
			if (coords[0] != coords[iCrd - 1])
				coords[iCrd++] = coords[0];
		}
		storeSegm(seg, coords, isIland, iCrd);
		//  aiSegNr &= sb.iSegNr;
	}
	else {
		// count chains
		int iChains = seg->dlChain.size();
		coords.resize(iChains + 1);
		// calculate coordinates
		Coordinate crdCurr = seg->crdFrom;
		coords[iCrd++] = crdCurr;
		for (ChainRec chain : seg->dlChain) {
			DirBound db = chain.dbCode;
			switch (db) {
			case dbRIGHT: {
				crdCurr.x += chain.iLength;
			}
						  break;
			case dbUP: {
				crdCurr.y -= chain.iLength;
			}
					   break;
			case dbLEFT: {
				crdCurr.x -= chain.iLength;
			}
						 break;
			case dbDOWN: {
				crdCurr.y += chain.iLength;
			}
						 break;
			default: assert(0 == 1);
				break;
			}
			coords[iCrd++] = crdCurr;
		}
	}

	storeSegm(seg, coords, isIland, iCrd);
}

RasterToPolygon::SegBoundPtr RasterToPolygon::newInBetween(int x, const std::vector<double>& inputLine, const std::vector<double>& inputLinePrev) {
	SegBoundPtr sb;
	sb.reset(new SegBound());
	//  sb->iForw = sb->iBackw = 0;
	sb->iSegNr = newSegNr();
	if (sb->iSegNr >= (int)_fwl.size()) {
		_fwl.resize(sb->iSegNr + 1);
		_bwl.resize(sb->iSegNr + 1);
	}
	_fwl[sb->iSegNr] = _bwl[sb->iSegNr] = 0;
	ChainRec crBeg, crEnd;
	if (_dirBoundsCurrent[x] == dbLEFT) {
		// left, down 
		crBeg.dbCode = dbLEFT;
		crEnd.dbCode = dbDOWN;
		sb->iLeftRaw = inputLine[x];
		sb->iRightRaw = inputLinePrev[x];
	}
	else { //up, right 
		crBeg.dbCode = dbUP;
		crEnd.dbCode = dbRIGHT;
		sb->iLeftRaw = inputLinePrev[x];
		sb->iRightRaw = inputLine[x];
	}
	sb->dlChain.push_front(crBeg); // insert at begin
	sb->dlChain.push_back(crEnd); // append to end
	sb->fBeginSeg = sb->fEndSeg = false;
	sb->crdFrom.x = sb->crdFrom.y = sb->crdTo.x = sb->crdTo.y = 0;
	return sb;
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
	else
		_segBoundsHoriz[x] = 0;
	if (segExist[dbUP]) { // end of segment up 
		sbSeg[dbUP] = _segBoundsVert[x];
		endOfSegment(x,y, sbSeg[dbUP], true, (bool &)beginSeg[dbUP]);
	}
	if (segExist[dbLEFT]) { //end of segment to the left 
		sbSeg[dbLEFT] = _segBoundsHoriz[x - 1];
		endOfSegment(x,y, sbSeg[dbLEFT], false, (bool &)beginSeg[dbLEFT]);
	}
	if (segExist[dbDOWN]) { // new segment down }
		sbSeg[dbDOWN] = newWithOneEnd(x, y, inputLine, inputLinePrev, false, (bool &)beginSeg[dbDOWN]);
		_segBoundsVert[x] = sbSeg[dbDOWN];
	}
	else
		_segBoundsVert[x] = 0;

	detLink(dbRIGHT, dbDOWN, dbLEFT, segExist, beginSeg, sbSeg);
	detLink(dbUP, dbRIGHT, dbDOWN, segExist, beginSeg, sbSeg);
	detLink(dbLEFT, dbUP, dbRIGHT, segExist, beginSeg, sbSeg);
	detLink(dbDOWN, dbLEFT, dbUP, segExist, beginSeg, sbSeg);

	if (segExist[dbUP]) // end of segment up 
		storeSegm(sbSeg[dbUP]);
	if (segExist[dbLEFT]) // end of segment to the left 
		storeSegm(sbSeg[dbLEFT]);
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

void RasterToPolygon::endOfSegment(int x, int y, RasterToPolygon::SegBoundPtr sb, bool fUp, bool& fBegin)
{
	if ((fUp && (_dirBoundsPrev[x] == dbUP)) ||
		(!fUp && (_dirBoundsCurrent[x - 1] == dbLEFT))) { // begin of segment
		fBegin = true;
		sb->fBeginSeg = true;
		sb->crdFrom.x = x; 
		sb->crdFrom.y = y;
	}
	else { // end of segment 
		fBegin = false;
		sb->fEndSeg = true;
		sb->crdTo.x = x; 
		sb->crdTo.y = y;
	}
}

void RasterToPolygon::detLink(DirBound db1, DirBound db2, DirBound db3,	const std::vector<bool>& segExist, const std::vector<bool>& beginSeg, const std::vector<SegBoundPtr>& sbSeg)
{
	long iSegNr;
	if (segExist[db1]) {
		if (segExist[db2]) {
			iSegNr = sbSeg[db2]->iSegNr;
			if (!beginSeg[db2])
				iSegNr = -iSegNr;
		}
		else {
			iSegNr = sbSeg[db3]->iSegNr;
			if (!beginSeg[db3])
				iSegNr = -iSegNr;
		}
		if (beginSeg[db1])
			_bwl[sbSeg[db1]->iSegNr] = iSegNr;
		else
			_fwl[sbSeg[db1]->iSegNr] = iSegNr;
	}
}

bool RasterToPolygon::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	// TODO : areanumbering

	IRasterCoverage areaNumberedRaster;

	int lineSize = _inputraster->size().xsize();
	std::vector<double> inputLine( lineSize + 2, rUNDEF); // lines are 2 cells bigger as a virtual cells are added to left and right
	std::vector<double> inputLinePrev(inputLine.size(), rUNDEF); // lines are 2 cells bigger as a virtual is added to left and right
	_dirBoundsNext.resize(inputLine.size(), dbNONE);
	_dirBoundsPrev.resize(inputLine.size(), dbNONE);
	_segBoundsHoriz.resize(inputLine.size());
	_segBoundsVert.resize(inputLine.size());
	_dirBoundsCurrent.resize(inputLine.size(), dbNONE);

	inputLine.front() = inputLine.back() = rUNDEF;

	PixelIterator iter(areaNumberedRaster);
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

void RasterToPolygon::linkBoundaries(const IRasterCoverage& areaNumberedRaster) {
	IItemDomain dom = areaNumberedRaster->datadef().domain().as<ItemDomain<DomainItem>>();
	int nrpol = dom->count();
	std::vector<bool> hasFwl(_fwl.size(), false);
	std::vector<bool> hasBwl(_bwl.size(), false);

	for (int i = 1; i <= nrpol; i++) {
		long iTopStart = _topStarts[i];
		long iTopCurr = iTopStart;
		do {
			if (iTopCurr > 0)
				hasFwl[iTopCurr] = true;
			else
				hasBwl[-iTopCurr] = true;
			iTopCurr = (iTopCurr > 0) ? _fwl[iTopCurr] : _bwl[-iTopCurr];
		} while (iTopCurr != iTopStart);
	}
	for (int i = 0; i < hasBwl.size(); ++i) {
		if (!hasBwl[i]) {
			long pol = _topology[i].second;
			if (pol > 0) {
				long iTopStart = _topStarts[pol];
				long iTopCurr;
				long iTopNext = iTopStart;
				do {
					iTopCurr = iTopNext;
					iTopNext = (iTopCurr > 0) ? _fwl[iTopCurr] : _bwl[-iTopCurr];
				} while (iTopNext != iTopStart);
				// topCurr is segment that links to ts (last segment of pol)
				// let fwl or bwl point to island
				if (iTopCurr > 0) {
					_fwl[iTopCurr] = -i;
				}
				else {
					_bwl[-iTopCurr] = -i;
				}
				// let island bwl point to ts
				if (iTopStart > 0) {
					_bwl[i] = iTopStart;
				}
				else {
					_bwl[i] = -iTopStart;
				}
			}
		}
		if (!_fwl[i]) {
			long pol = _topology[i].first; // iLeftRaw
			if (pol > 0) {
				// topology should be linked in polygon
				long iTopStart = _topStarts[pol];
				long iTopCurr;
				long iTopNext = iTopStart;
				do {
					iTopCurr = iTopNext;
					iTopNext = (iTopCurr > 0) ? _fwl[iTopCurr] : _bwl[-iTopCurr];
				} while (iTopNext != iTopStart);
				// topCurr is segment that links to ts (last segment of pol)
				// let fwl or bwl point to island
				if (iTopCurr > 0) {
					_fwl[iTopCurr] = i;
				}
				else {
					_bwl[-iTopCurr] = i;
				}
				// let island fwl point to ts
				if (iTopStart > 0) {
					_fwl[i] = iTopStart;
				}
				else {
					_bwl[i] = -iTopStart;
				}
			}
		}
	}
	/*int i = 0;
	for (std::map<long, long>::iterator topIter = _topStarts.begin(); topIter != topStarts.end(); ++topIter) {
		std::vector<geos::geom::CoordinateSequence*> coords;
		long iRaw = topIter->first;
		long iTopStart = topIter->second;
		long iTopCurr = iTopStart;
		do {
			if (coords.size() == 0 || !appendCoords(coords[coords.size() - 1], topologySegments[abs(iTopCurr)], iTopCurr > 0)) {
				geos::geom::CoordinateSequence* seg = topologySegments[abs(iTopCurr)].clone();
				if (iTopCurr < 0)
					geos::geom::CoordinateSequence::reverse(seg);
				coords.push_back(seg);
			}
			iTopCurr = (iTopCurr > 0) ? _fwl[iTopCurr] : _bwl[-iTopCurr];
		} while (iTopCurr != iTopStart);

		autocorrectCoords(coords);

		std::vector<std::pair<geos::geom::LinearRing *, std::vector<geos::geom::Geometry *> *>> polys = makePolys(coords);
		for (std::vector<std::pair<geos::geom::LinearRing *, std::vector<geos::geom::Geometry *> *>>::iterator poly = polys.begin(); poly != polys.end(); ++poly) {
			geos::geom::LinearRing * ring = poly->first;
			std::vector<geos::geom::Geometry *> * holes = poly->second;
			geos::geom::Polygon * gpol = _outputfeatures->geomfactory()->createPolygon(ring, holes); // takes ownership of both ring and holes pointers
			auto feature = _outputfeatures->newFeature({ (geos::geom::Geometry *)gpol }, false);
			if (dvrs().fRealValues()) {
				double r = colAtt->rValue(iRaw);
				pol->PutVal(r);
				//if (r == rUNDEF)
				//	pol->Delete(true);
			}
			else {
				long v = colAtt->iRaw(iRaw);
				if (v == iUNDEF + 1) // area numbering changes iUNDEF to iUNDEF + 1 (it is the current logic of that application)
					v = iUNDEF;
				pol->PutVal(v);
				//if (v == iUNDEF)
				//	pol->Delete(true);
			}
		}
		++i;
	}*/
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

