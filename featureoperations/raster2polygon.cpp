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
#include "geos/geom/Polygon.h"
#include "geos/geom/Geometry.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/LinearRing.h"
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
#include "ilwis3buf.h"

using namespace Ilwis;
using namespace Ilwis3;

#include "raster2polygon.h"

using namespace FeatureOperations;

CoordBuf::CoordBuf() : CoordinateArraySequence() {}
CoordBuf::CoordBuf(int isz) : CoordinateArraySequence(isz) {}

CoordBuf::CoordBuf(const CoordinateSequence* seq) : CoordinateArraySequence() {
	for (int i = 0; i < seq->size(); ++i) {
		add(seq->getAt(i));
	}
}

Coordinate& CoordBuf::operator[](int i) {
	return const_cast<Coordinate&>((const Coordinate&)getAt(i));
}

Coordinate CoordBuf::operator[](int i) const {
	return (const Coordinate&)getAt(i);
}

void CoordBuf::Size(int sz) {
	if (sz > size()) {
		for (int i = size(); i < sz; ++i) {
			add(Coordinate(rUNDEF, rUNDEF));
		}
	}
	else if (sz < size()) {
		for (int i = size() - 1; i >= sz; --i) {
			deleteAt(i);
		}
	}
}

CoordBuf& CoordBuf::operator=(const CoordBuf& b) {
	Size(b.size());
	for (int i = 0; i < b.size(); ++i) {
		setAt(b[i], i);
	}
	return *this;
}

CoordBuf& CoordBuf::operator=(const CoordinateSequence* b) {
	Size(b->size());
	for (int i = 0; i < b->size(); ++i) {
		setAt(b->getAt(i), i);
	}
	return *this;
}

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

bool RasterToPolygon::fFindBoundaries(const Ilwis::IRasterCoverage& mpAreas)
{
	long iLines = _inputraster->size().ysize();
	long iCols = _inputraster->size().xsize();
	iBufInp.Size(iCols, 1, 1);  // rasvec var: line
	iBufInpPrev.Size(iCols, 1, 1); // rasvec var: last_line
	dbBufPrev.Size(iCols, 1, 1); // rasvec var: prev
	dbBufCurr.Size(iCols, 1, 1); // rasvec var: cur
	dbBufNext.Size(iCols, 1, 1); // rasvec var: next
	sbHoriz.Size(iCols, 1, 1); // rasvec var: segline[RIGHT]
	sbVert.Size(iCols, 1, 1); // rasvec var: segline[DOWN]

	iNrSeg = 0;

	long i, j;
	for (j = -1; j <= iCols; ++j)
	{
		iBufInpPrev[j] = iUNDEF;
		dbBufPrev[j] = dbNONE;
		sbHoriz[j] = 0;
		sbVert[j] = 0;
	}
	initialize(iLines);
	PixelIterator iterIn(mpAreas);

	for (i = 0; i < iLines + 1; ++i)
	{
		trq()->update(1);
		if (i < iLines) {
			for (long ii = 0; ii < iCols; ++ii) {
				double val = *iterIn;
				iBufInp[ii] = (val != rUNDEF) ? (val + 1) : iUNDEF; // the ilwis3 code expects raw values to start at 1
				++iterIn;
			}
		}
		else {
			for (long ii = 0; ii < iCols; ++ii)
				iBufInp[ii] = iUNDEF;
		}

		iBufInp[-1] = iUNDEF;
		iBufInp[iCols] = iUNDEF;
		// calc dbBufCurr with horizontal directions by comparing vertical differences of pixel rows
		// if upper pixel > lower pixel then direction is RIGHT
		if (i == iLines + 1)
			for (j = -1; j <= iCols; ++j)
				iBufInp[j] = iUNDEF;   //  fill the extra bottom line with Undefs
		for (j = -1; j <= iCols; ++j)
		{
			if (iBufInpPrev[j] == iBufInp[j])
				dbBufCurr[j] = dbNONE;
			else if (iBufInpPrev[j] > iBufInp[j])
				dbBufCurr[j] = dbRIGHT;
			else
				dbBufCurr[j] = dbLEFT;
		}
		// calc dbBufNext with vertical directions by comparing horizontal differences of pixel columns
		// if left pixel > right pixel then direction is UP
		dbBufNext[-1] = dbNONE;
		for (j = 0; j <= iCols; ++j)
		{
			if (iBufInp[j - 1] == iBufInp[j])
				dbBufNext[j] = dbNONE;
			else if (iBufInp[j - 1] > iBufInp[j])
				dbBufNext[j] = dbUP;
			else
				dbBufNext[j] = dbDOWN;
		}

		for (j = 0; j <= iCols; ++j)
		{
			byte b = 0;
			// b indicates for 4 pixel boundaries where pixels are different
			//
			//          2
			//     pix  |  pix
			//          |
			//     4 ------- 1
			//          |
			//     pix  |  pix
			//          8
			//
			if (dbBufCurr[j] != dbNONE)
				b |= 1;
			if (dbBufPrev[j] != dbNONE)
				b |= 2;
			if (dbBufCurr[j - 1] != dbNONE)
				b |= 4;
			if (dbBufNext[j] != dbNONE)
				b |= 8;
			switch (b)
			{
			case 0:
			{
				sbHoriz[j] = 0;
				sbVert[j] = 0;
			}
			break;
			case 7: case 11: case 13: case 14:
			{
				NewNode(i, j, b);
			}
			break;
			case 15:
			{
				if (!_fEightCon)
					NewNode(i, j, b);
				else
				{
					bool f1 = iBufInpPrev[j] == iBufInp[j - 1];
					bool f2 = iBufInpPrev[j - 1] == iBufInp[j];
					if (f1 && f2)
					{
						if (iBufInp[j - 1] > iBufInp[j])
							f2 = false;
						else
							f1 = false;
					}
					if (f1)
					{
						AppendLeftUp(i, j); // b = 6
						//b = 9
						sbVert[j] = sbHoriz[j] = sbNewInBetween(j);

					}
					else if (f2)
					{
						AppendUp(i, j, 1/*3*/);
						SegBound* h = sbHoriz[j];
						AppendLeft(i, j, 0/*12*/);
						sbHoriz[j] = h;
					}
					else
						NewNode(i, j, b);
				}
				break;
			}
			case 3: case 10: // append to upper segment
			{
				AppendUp(i, j, b);
			}
			break;
			case 5: case 12: // append to left segment
			{
				AppendLeft(i, j, b);
			}
			break;
			case 6: //  append to left and up (become same segment)
			{
				AppendLeftUp(i, j);
			}
			break;
			case 9:
			{
				sbVert[j] = sbHoriz[j] = sbNewInBetween(j);
			}
			break;
			case 1: case 2: case 4: case 8: // does not occur
				break;
			}
		}
		// swap buffers to be able to use them in the next line
		Swap(iBufInp, iBufInpPrev);
		Swap(dbBufNext, dbBufPrev);
	}
	return true;
}

bool RasterToPolygon::appendCoords(geos::geom::CoordinateSequence*& coordsA, geos::geom::CoordinateSequence& coordsB, bool fForward) const
{
	if (fForward ? (coordsA->back() == coordsB.front()) : (coordsA->back() == coordsB.back())) {
		coordsA->add(&coordsB, false, fForward);
		return true;
	}
	else
		return false;
}

void RasterToPolygon::autocorrectCoords(std::vector<geos::geom::CoordinateSequence*>& coords) const
{
	bool fChanged;
	std::vector<long> openCoords;
	for (long i = 0; i < coords.size(); ++i) { // mark all open coordinatesequences (back != front)
		if (coords[i]->back() != coords[i]->front())
			openCoords.push_back(i);
	}
	do { // link all possible backs to fronts, creating closed coordinatesequences that can become polygons
		fChanged = false;
		for (long i = 0; i < openCoords.size(); ++i) {
			for (long j = i + 1; j < openCoords.size(); ++j) {
				if (appendCoords(coords[openCoords[i]], *coords[openCoords[j]], true)) {
					delete coords[openCoords[j]];
					coords.erase(coords.begin() + openCoords[j]);
					openCoords.erase(openCoords.begin() + j);
					for (long k = j; k < openCoords.size(); ++k) // shift all indexes
						openCoords[k] = openCoords[k] - 1;
					j = j - 1;
					fChanged = true;
				}
			}
			if (coords[openCoords[i]]->back() == coords[openCoords[i]]->front()) {
				openCoords.erase(openCoords.begin() + i);
				i = i - 1;
			}
		}
	} while (fChanged);
	for (long i = 0; i < openCoords.size(); ++i) { // delete all remaining open coordinatesequences
		delete coords[openCoords[i]];
		coords.erase(coords.begin() + openCoords[i]);
		for (long k = i; k < openCoords.size(); ++k) // shift all indexes
			openCoords[k] = openCoords[k] - 1;
	}
}

std::vector<std::pair<geos::geom::LinearRing*, std::vector<geos::geom::Geometry*>*>> RasterToPolygon::makePolys(std::vector<geos::geom::CoordinateSequence*>& coords, geos::geom::GeometryFactory* fact) const
{
	std::vector<std::pair<geos::geom::LinearRing*, std::vector<geos::geom::Geometry*>*>> result;
	CachedRelation relation;

	// CoordSequence to Polygons
	std::vector<geos::geom::Polygon*> rings;
	for (std::vector<geos::geom::CoordinateSequence*>::iterator coordsN = coords.begin(); coordsN != coords.end(); ++coordsN)
		rings.push_back(fact->createPolygon(new geos::geom::LinearRing(*coordsN, fact), 0));

	std::multimap<long, geos::geom::Polygon*> levels;
	long iMaxDepth = 0;

	// find the winding level of each ring
	for (long i = 0; i < rings.size(); ++i) {
		geos::geom::Polygon* ringI = rings[i];
		long iDepth = 0;
		for (long j = 0; j < rings.size(); ++j) {
			if (j == i) // test against every other ring, except itself
				continue;
			geos::geom::Polygon* ringJ = rings[j];
			if (relation.within(ringI, ringJ))
				++iDepth;
		}
		levels.insert(std::pair<long, geos::geom::Polygon*>(iDepth, ringI));
		iMaxDepth = max(iMaxDepth, iDepth);
	}

	// go through all exterior rings and collect their holes; EVEN depths become exterior, ODD depths become holes, each hole is added only to its closest exterior ring
	for (long iDepth = 0; iDepth <= iMaxDepth; iDepth += 2) {
		std::pair<std::multimap<long, geos::geom::Polygon*>::iterator, std::multimap<long, geos::geom::Polygon*>::iterator> exteriors = levels.equal_range(iDepth);
		std::pair<std::multimap<long, geos::geom::Polygon*>::iterator, std::multimap<long, geos::geom::Polygon*>::iterator> holes = levels.equal_range(iDepth + 1);
		for (std::multimap<long, geos::geom::Polygon*>::iterator exteriorIt = exteriors.first; exteriorIt != exteriors.second; ++exteriorIt) {
			std::vector<geos::geom::Geometry*>* ringHoles = new std::vector<geos::geom::Geometry*>();
			for (std::multimap<long, geos::geom::Polygon*>::iterator holeIt = holes.first; holeIt != holes.second; ++holeIt) {
				if (relation.within(holeIt->second, exteriorIt->second))
					ringHoles->push_back((geos::geom::Geometry*)holeIt->second->getExteriorRing());
			}
			result.push_back(std::pair<geos::geom::LinearRing*, std::vector<geos::geom::Geometry*> *>((geos::geom::LinearRing*)exteriorIt->second->getExteriorRing(), ringHoles));
		}
	}

	return result;
}

std::map<long, geos::geom::Geometry*> RasterToPolygon::makeMultiPolys(std::map<long, std::vector<geos::geom::Geometry*>> polygons, geos::geom::GeometryFactory* fact)
{
	std::map<long, geos::geom::Geometry*> multiPolygons;
	for (std::map<long, std::vector<geos::geom::Geometry*>>::iterator gpol = polygons.begin(); gpol != polygons.end(); ++gpol) {
		long iRaw = gpol->first;
		std::vector<geos::geom::Geometry*> polys = gpol->second;
		geos::geom::Geometry * multiPoly = fact->buildGeometry(polys);
		multiPolygons[iRaw] = multiPoly;
		for (std::vector<geos::geom::Geometry*>::iterator pol = polys.begin(); pol != polys.end(); ++pol)
			delete* pol;
	}
	return multiPolygons;
}

void RasterToPolygon::DetLink(DirBound db1, DirBound db2, DirBound db3,
	const std::vector<int>& fSegExist, const std::vector<int>& fBeginSeg,
	const std::vector<SegBound*>& sbSeg)
{
	long iSegNr;
	if (fSegExist[db1]) {
		if (fSegExist[db2]) {
			iSegNr = sbSeg[db2]->iSegNr;
			if (!fBeginSeg[db2])
				iSegNr = -iSegNr;
		}
		else {
			iSegNr = sbSeg[db3]->iSegNr;
			if (!fBeginSeg[db3])
				iSegNr = -iSegNr;
		}
		if (fBeginSeg[db1])
			aiBwl[sbSeg[db1]->iSegNr] = iSegNr;
		else
			aiFwl[sbSeg[db1]->iSegNr] = iSegNr;
	}
}

void RasterToPolygon::NewNode(long iLine, long iCol, byte b)
{
	std::vector<int> fSegExist(dbRIGHT + 1);
	std::vector<int> fBeginSeg(dbRIGHT + 1);
	std::vector<SegBound*> sbSeg(dbRIGHT + 1);
	fSegExist[dbRIGHT] = b & 1;
	fSegExist[dbUP] = (b & 2) != 0;
	fSegExist[dbLEFT] = (b & 4) != 0;
	fSegExist[dbDOWN] = (b & 8) != 0;
	if (fSegExist[dbRIGHT]) {// new segment to the right
		sbSeg[dbRIGHT] = sbNewWithOneEnd(iLine, iCol, true, fBeginSeg[dbRIGHT]);
		sbHoriz[iCol] = sbSeg[dbRIGHT];
	}
	else
		sbHoriz[iCol] = 0;
	if (fSegExist[dbUP]) { // end of segment up
		sbSeg[dbUP] = sbVert[iCol];
		EndOfSegment(iLine, iCol, *sbSeg[dbUP], true, fBeginSeg[dbUP]);
	}
	if (fSegExist[dbLEFT]) { //end of segment to the left
		sbSeg[dbLEFT] = sbHoriz[iCol - 1];
		EndOfSegment(iLine, iCol, *sbSeg[dbLEFT], false, fBeginSeg[dbLEFT]);
	}
	if (fSegExist[dbDOWN]) { // new segment down }
		sbSeg[dbDOWN] = sbNewWithOneEnd(iLine, iCol, false, fBeginSeg[dbDOWN]);
		sbVert[iCol] = sbSeg[dbDOWN];
	}
	else
		sbVert[iCol] = 0;

	DetLink(dbRIGHT, dbDOWN, dbLEFT, fSegExist, fBeginSeg, sbSeg);
	DetLink(dbUP, dbRIGHT, dbDOWN, fSegExist, fBeginSeg, sbSeg);
	DetLink(dbLEFT, dbUP, dbRIGHT, fSegExist, fBeginSeg, sbSeg);
	DetLink(dbDOWN, dbLEFT, dbUP, fSegExist, fBeginSeg, sbSeg);

	if (fSegExist[dbUP]) // end of segment up
		StoreSegm(*sbSeg[dbUP]);
	if (fSegExist[dbLEFT]) // end of segment to the left
		StoreSegm(*sbSeg[dbLEFT]);
}


void RasterToPolygon::AppendLeftUp(long iLine, long iCol)
/* Appends codes of point (iLine, iCol) to segment to the left of point.
   The segment above the point is chained to the left segment.
   Is called if type of point = 6 (line from left to up or line
   from up to left.                                              */
{
	enum WhatDel { DELNONE, DELLEFT, DELUP } del;
	SegBound* sbUp = sbVert[iCol];
	SegBound* sbLeft = sbHoriz[iCol - 1];
	if (sbLeft != sbUp) {
		del = DELNONE;
		if (dbBufCurr[iCol - 1] == dbLEFT) { //line comes from above, goes left }
			if (sbUp->fBeginSeg) {
				for (std::vector<ChainRec>::iterator it = sbLeft->dlChain.begin(); it != sbLeft->dlChain.end(); ++it)
					sbUp->dlChain.push_back(*it);
				//        for (DLIter<ChainRec> iter(&sbLeft->dlChain); iter.fValid(); iter++)
				//          sbUp->dlChain.append(iter());
				del = DELLEFT;
				sbUp->fEndSeg = sbLeft->fEndSeg;
				if (sbUp->fEndSeg) {
					aiFwl[sbUp->iSegNr] = aiFwl[sbLeft->iSegNr];
					for (unsigned long i = 0; i < aiBwl.size(); ++i) {
						if (aiBwl[i] == -sbLeft->iSegNr)
							aiBwl[i] = -sbUp->iSegNr;
						else if (aiBwl[i] == sbLeft->iSegNr)
							aiBwl[i] = sbUp->iSegNr;
						else if (aiFwl[i] == -sbLeft->iSegNr)
							aiFwl[i] = -sbUp->iSegNr;
						else if (aiFwl[i] == sbLeft->iSegNr)
							aiFwl[i] = sbUp->iSegNr;
					}
				}
				sbUp->crdTo = sbLeft->crdTo;
				StoreSegm(*sbUp);
			}
			else { // sbUp->fBeginSeg == false
				for (std::vector<ChainRec>::reverse_iterator it = sbUp->dlChain.rbegin(); it != sbUp->dlChain.rend(); ++it)
					sbLeft->dlChain.insert(sbLeft->dlChain.begin(), *it);
				del = DELUP;
			}
		}
		else { // line comes from left, goes to above
			if (sbUp->fEndSeg) {
				for (std::vector<ChainRec>::reverse_iterator it = sbLeft->dlChain.rbegin(); it != sbLeft->dlChain.rend(); ++it)
					sbUp->dlChain.insert(sbUp->dlChain.begin(), *it);
				del = DELLEFT;
				sbUp->fBeginSeg = sbLeft->fBeginSeg;
				if (sbUp->fBeginSeg) {
					aiBwl[sbUp->iSegNr] = aiBwl[sbLeft->iSegNr];
					for (unsigned long i = 0; i < aiFwl.size(); ++i) {
						if (aiFwl[i] == sbLeft->iSegNr)
							aiFwl[i] = sbUp->iSegNr;
						else if (aiFwl[i] == -sbLeft->iSegNr)
							aiFwl[i] = -sbUp->iSegNr;
						else if (aiBwl[i] == sbLeft->iSegNr)
							aiBwl[i] = sbUp->iSegNr;
						else if (aiBwl[i] == -sbLeft->iSegNr)
							aiBwl[i] = -sbUp->iSegNr;
					}
				}
				sbUp->crdFrom = sbLeft->crdFrom;
				StoreSegm(*sbUp);
			}
			else {
				for (std::vector<ChainRec>::iterator it = sbUp->dlChain.begin(); it != sbUp->dlChain.end(); ++it)
					sbLeft->dlChain.push_back(*it);
				del = DELUP;
			}
		}
		// adjust sbHoriz and sbVert by replacing the deleted SegBound with the other one
		if (del == DELLEFT) {
			for (long i = sbHoriz.iLower(); i <= sbHoriz.iUpper(); i++) {
				if (sbHoriz[i] == sbLeft)
					sbHoriz[i] = sbUp;
				if (sbVert[i] == sbLeft)
					sbVert[i] = sbUp;
			}
			aiSegNr.push_back(sbLeft->iSegNr);
			aiFwl[sbLeft->iSegNr] = 0;
			aiBwl[sbLeft->iSegNr] = 0;
			delete sbLeft;
		}
		else if (del == DELUP) {
			for (long i = sbHoriz.iLower(); i <= sbHoriz.iUpper(); i++) {
				if (sbHoriz[i] == sbUp)
					sbHoriz[i] = sbLeft;
				if (sbVert[i] == sbUp)
					sbVert[i] = sbLeft;
			}
			aiSegNr.push_back(sbUp->iSegNr);
			aiFwl[sbUp->iSegNr] = 0;
			aiBwl[sbUp->iSegNr] = 0;
			delete sbUp;
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
		sbUp->crdFrom.x = iCol; sbUp->crdFrom.y = iLine;
		sbUp->fEndSeg = true;
		sbUp->crdTo.x = iCol; sbUp->crdTo.y = iLine;
		aiFwl[sbUp->iSegNr] = sbUp->iSegNr;
		aiBwl[sbUp->iSegNr] = -sbUp->iSegNr;
		StoreSegm(*sbUp);
	}
	sbHoriz[iCol] = 0;
	sbVert[iCol] = 0;
}

void RasterToPolygon::AppendUp(long iLine, long iCol, byte b)
// Append chain codes of (iLine,iRow) to segment above point
{
	SegBound* sb = sbVert[iCol];
	DirBound db;
	if ((dbBufCurr[iCol] == dbLEFT) || ((b != 1)) && (dbBufNext[iCol] == dbUP)) {
		// line goes up
		if (b & 1)
			db = dbLEFT;
		else
			db = dbUP;
		if (sb->crFirst().dbCode == db)
			sb->crFirst().iLength++;
		else
			sb->Insert(db, 1); // insert at begin
	}
	else { // line goes down
		if (b & 1)
			db = dbRIGHT;
		else
			db = dbDOWN;
		if (sb->dlChain.back().dbCode == db)
			sb->crLast().iLength++;
		else
			sb->Append(db, 1); // append at end
	}
	if (b == 10) {
		sbHoriz[iCol] = 0;
		sbVert[iCol] = sb;
	}
	else {
		sbVert[iCol] = 0;
		sbHoriz[iCol] = sb;
	}
	StoreSegm(*sb);
}

void RasterToPolygon::AppendLeft(long iLine, long iCol, byte b)
// Append chain code from (iLine, iCol) to segment to the left.
{
	SegBound* sb = sbHoriz[iCol - 1];
	DirBound db;
	if (((dbBufCurr[iCol] == dbLEFT) && (b != 0)) || (dbBufNext[iCol] == dbUP)) {
		// line goes left
		if (b & 1)
			db = dbLEFT;
		else
			db = dbUP;
		if (sb->crFirst().dbCode == db)
			sb->crFirst().iLength++;
		else
			sb->Insert(db, 1); // insert at begin
	}
	else { // line goes right
		if (b & 1)
			db = dbRIGHT;
		else
			db = dbDOWN;
		if (sb->dlChain.back().dbCode == db)
			sb->crLast().iLength++;
		else
			sb->Append(db, 1); // append at end
	}
	if (b == 5) {
		sbVert[iCol] = 0;
		sbHoriz[iCol] = sb;
	}
	else {
		sbHoriz[iCol] = 0;
		sbVert[iCol] = sb;
	}
	StoreSegm(*sb);
}

SegBound* RasterToPolygon::sbNewInBetween(long iCol)
// Creates a new segment without nodes but with two chain codes.
// Is called if point has code 9 ((left,down) or (up,right)).
{
	SegBound* sb = new SegBound;
	//  sb->iForw = sb->iBackw = 0;
	sb->iSegNr = iNewSegNr();
	if (sb->iSegNr >= (long)aiFwl.size()) {
		aiFwl.resize(sb->iSegNr + 1);
		aiBwl.resize(sb->iSegNr + 1);
	}
	aiFwl[sb->iSegNr] = aiBwl[sb->iSegNr] = 0;
	ChainRec crBeg, crEnd;
	if (dbBufCurr[iCol] == dbLEFT) {
		// left, down
		crBeg.dbCode = dbLEFT;
		crEnd.dbCode = dbDOWN;
		sb->iLeftRaw = iBufInp[iCol];
		sb->iRightRaw = iBufInpPrev[iCol];
	}
	else { //up, right
		crBeg.dbCode = dbUP;
		crEnd.dbCode = dbRIGHT;
		sb->iLeftRaw = iBufInpPrev[iCol];
		sb->iRightRaw = iBufInp[iCol];
	}
	sb->dlChain.insert(sb->dlChain.begin(), crBeg); // insert at begin
	sb->dlChain.push_back(crEnd); // append to end
	sb->fBeginSeg = sb->fEndSeg = false;
	sb->crdFrom.x = sb->crdFrom.y = sb->crdTo.x = sb->crdTo.y = 0;
	return sb;
}

SegBound* RasterToPolygon::sbNewWithOneEnd(long iLine, long iCol, bool fRight, int& fBegin)
// Creates new segment with a node at one end.
// If fRightSeg==true : it has to be a segment to the right of node,
// else a segment under the node.
{
	DirBound db;
	if (fRight)
		db = (DirBound)dbBufCurr[iCol];
	else
		db = (DirBound)dbBufNext[iCol];
	SegBound* sb = new SegBound;
	//  sb->iForw = sb->iBackw = 0;
	sb->iSegNr = iNewSegNr();
	if (sb->iSegNr >= (long)aiFwl.size()) {
		aiFwl.resize(sb->iSegNr + 1);
		aiBwl.resize(sb->iSegNr + 1);
	}
	aiFwl[sb->iSegNr] = aiBwl[sb->iSegNr] = 0;
	if ((db == dbRIGHT) || (db == dbDOWN)) { // start of segment
		sb->Insert(db, 1); // at start
		sb->fBeginSeg = true;
		fBegin = true;
		sb->fEndSeg = false;
		sb->crdFrom.x = iCol; sb->crdFrom.y = iLine;
		sb->crdTo.x = sb->crdTo.y = 0;
	}
	else { // end of segment
		sb->Append(db, 1); // at end
		sb->fBeginSeg = false;
		fBegin = false;
		sb->fEndSeg = true;
		sb->crdFrom.x = sb->crdFrom.y = 0;
		sb->crdTo.x = iCol; sb->crdTo.y = iLine;
	}
	switch (db) {
	case dbRIGHT: {
		//            top.PolL := last_line^[x]; top.PolR := line^[x];
		sb->iLeftRaw = iBufInpPrev[iCol];
		sb->iRightRaw = iBufInp[iCol];
	}
				break;
	case dbUP: {
		//              top.PolL := line^[x - 1]; top.PolR := line^[x];
		sb->iLeftRaw = iBufInp[iCol - 1];
		sb->iRightRaw = iBufInp[iCol];
	}
			 break;
	case dbLEFT: {
		//              top.PolL := line^[x]; top.PolR := last_line^[x];
		sb->iLeftRaw = iBufInp[iCol];
		sb->iRightRaw = iBufInpPrev[iCol];
	}
			   break;
	case dbDOWN: {
		//                top.PolL := line^[x]; top.PolR := line^[x - 1];
		sb->iLeftRaw = iBufInp[iCol];
		sb->iRightRaw = iBufInp[iCol - 1];
	}
			   break;
	default: assert(0 == 1);
		break;
	}
	return sb;
}

void RasterToPolygon::EndOfSegment(long iLine, long iCol, SegBound& sb, bool fUp, int& fBegin)
{
	if ((fUp && (dbBufPrev[iCol] == dbUP)) ||
		(!fUp && (dbBufCurr[iCol - 1] == dbLEFT))) { // begin of segment
		fBegin = true;
		sb.fBeginSeg = true;
		sb.crdFrom.x = iCol; sb.crdFrom.y = iLine;
	}
	else { // end of segment
		fBegin = false;
		sb.fEndSeg = true;
		sb.crdTo.x = iCol; sb.crdTo.y = iLine;
	}
}

void RasterToPolygon::StoreSegm(SegBound& sb)
{
	if (!sb.fBeginSeg || !sb.fEndSeg)
		return;
	CoordBuf cBuf;
	long iCrd = 0;
	bool fIsland = sb.crdFrom == sb.crdTo;
	if (_fSmooth) {
		// calculate coordinates
		Coordinate crdFrom = sb.crdFrom;
		Coordinate crdTo = sb.crdTo;
		std::vector<ChainRec> acr;
		for (std::vector<ChainRec>::iterator it = sb.dlChain.begin(); it != sb.dlChain.end(); ++it)
			acr.push_back(*it);
		cBuf.Size(acr.size() * 2); // should be sufficient
		double x, y, dx, dy;
		x = crdFrom.x;
		y = crdFrom.y;
		long iPrevStep, iCurrStep, iNextStep;
		if (fIsland) {
			ChainRec crLast = acr[acr.size() - 1];
			iPrevStep = crLast.iLength;
			if ((crLast.dbCode == dbUP) || (crLast.dbCode == dbLEFT))
				iPrevStep = -iPrevStep;
		}
		else {
			iPrevStep = 0;
			cBuf[iCrd++] = crdFrom;
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
				if (fIsland) {
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
					cBuf[iCrd++] = Coordinate(x + dx / 2, y + dy / 2);
					iNewCrd++;
				}
				if (!fIsland && (iNextStep == 0)) {
					cBuf[iCrd++] = Coordinate(x + dx, y + dy);
					iNewCrd++;
				}
			}
			else {
				if ((abs(iPrevStep) == 1) || (iNextStep == 1)) {
					cBuf[iCrd++] = Coordinate(x + dx / 2, y + dy / 2);
					iNewCrd++;
				}
				if (abs(iNextStep) != 1) {
					cBuf[iCrd++] = Coordinate(x + dx, y + dy);
					iNewCrd++;
				}
			}
			if (fIsland && (iCrd == iNewCrd))
				cFirst = cBuf[0];
			x += dx;
			y += dy;
			iPrevStep = iCurrStep;
			iCurrStep = iNextStep;
		}
		if (fIsland) {
			if (cBuf[0] != cBuf[iCrd - 1])
				cBuf[iCrd++] = cBuf[0];
		}
		StoreSegm(sb, cBuf, iCrd);
		//  aiSegNr &= sb.iSegNr;
	}
	else {
		// count chains
		int iChains = 0;
		for (std::vector<ChainRec>::iterator it = sb.dlChain.begin(); it != sb.dlChain.end(); ++it)
			++iChains;
		cBuf.Size(iChains + 1);
		// calculate coordinates
		Coordinate crdCurr = sb.crdFrom;
		cBuf[iCrd++] = crdCurr;
		for (std::vector<ChainRec>::iterator iter = sb.dlChain.begin(); iter != sb.dlChain.end(); ++iter) {
			DirBound db = iter->dbCode;
			switch (db) {
			case dbRIGHT: {
				crdCurr.x += iter->iLength;
			}
						break;
			case dbUP: {
				crdCurr.y -= iter->iLength;
			}
					 break;
			case dbLEFT: {
				crdCurr.x -= iter->iLength;
			}
					   break;
			case dbDOWN: {
				crdCurr.y += iter->iLength;
			}
					   break;
			default: assert(0 == 1);
				break;
			}
			cBuf[iCrd++] = crdCurr;
		}
	}

	StoreSegm(sb, cBuf, iCrd);
}

void RasterToPolygon::StoreSegm(const SegBound& sb, CoordBuf& cBuf, long& iCrd)
{
	if (iCrd == 0)
		return;

	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	trq()->stop(_startClock, end);
	for (long j = 0; j < iCrd; ++j)
		cBuf[j] = _inputraster->georeference()->pixel2Coord(Pixeld(cBuf[j].x, cBuf[j].y));
	topologySegments[sb.iSegNr] = CoordBuf(iCrd); // for speed optimization in ilwis 2.x the cBuf.size() didn't correspond to iCrd; iCrd is the actual size
	CoordBuf& cBufTemp = topologySegments[sb.iSegNr];
	for (long j = 0; j < iCrd; ++j)
		cBufTemp[j] = cBuf[j];
	topology[sb.iSegNr] = std::pair<long, long>(sb.iLeftRaw, sb.iRightRaw);
	if (sb.iLeftRaw > 0)
		topStarts[sb.iLeftRaw] = sb.iSegNr;
	iCrd = 0;
}

long RasterToPolygon::iNewSegNr()
{
	long iInd = aiSegNr.size() - 1;
	if (iInd >= 0) {
		long iSegNr = aiSegNr[iInd];
		aiSegNr.erase(aiSegNr.begin() + iInd);
		return iSegNr;
	}
	return ++iNrSeg;
}

bool RasterToPolygon::execute(ExecutionContext* ctx, SymbolTable& symTable) {
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	std::string connectivity = _fEightCon ? "8" : "4";
	std::string output_name = "areanumbering_object_" + QString::number(Ilwis::Identity::newAnonymousId()).toStdString();
	QString expr = QString::fromStdString(output_name + "=areanumbering(" + _inputraster->name().toStdString() + "," + connectivity + ",true)");
	Ilwis::commandhandler()->execute(expr, ctx, symTable);

	Ilwis::Symbol result = symTable.getSymbol(ctx->_results[0]);
	if (result._type == itRASTER && result._var.canConvert<Ilwis::IRasterCoverage>()) {
		Ilwis::IRasterCoverage mapArn(result._var.value<Ilwis::IRasterCoverage>());

		if (!fFindBoundaries(mapArn))
			return false;
		iNrPol = topStarts.size();
		// link islands to polygons
		long i;
		std::vector<bool> afFwl(aiFwl.size()), afBwl(aiFwl.size());
		for (i = 1; i < afFwl.size(); i++) {
			afFwl[i] = false;
			afBwl[i] = false;
		}
		trq()->inform("Check Islands\n");
		initialize(iNrPol);
		for (i = 1; i <= iNrPol; i++) {
			trq()->update(1);
			long iTopStart = topStarts[i];
			long iTopCurr = iTopStart;
			do {
				if (iTopCurr > 0)
					afFwl[iTopCurr] = true;
				else
					afBwl[-iTopCurr] = true;
				iTopCurr = (iTopCurr > 0) ? aiFwl[iTopCurr] : aiBwl[-iTopCurr];
			} while (iTopCurr != iTopStart);
		}
		trq()->inform("Link Islands\n");
		initialize(afBwl.size());
		for (i = 1; i <= afBwl.size(); ++i) {
			trq()->update(1);
			if (!afBwl[i-1]) {
				long pol = topology[i].second; // iRightRaw
				if (pol > 0) {
					// topology should be linked in polygon
					long iTopStart = topStarts[pol];
					long iTopCurr;
					long iTopNext = iTopStart;
					do {
						iTopCurr = iTopNext;
						iTopNext = (iTopCurr > 0) ? aiFwl[iTopCurr] : aiBwl[-iTopCurr];
					} while (iTopNext != iTopStart);
					// topCurr is segment that links to ts (last segment of pol)
					// let fwl or bwl point to island
					if (iTopCurr > 0) {
						aiFwl[iTopCurr] = -i;
					}
					else {
						aiBwl[-iTopCurr] = -i;
					}
					// let island bwl point to ts
					if (iTopStart > 0) {
						aiBwl[i] = iTopStart;
					}
					else {
						aiBwl[i] = -iTopStart;
					}
				}
			}
			if (!afFwl[i-1]) {
				long pol = topology[i].first; // iLeftRaw
				if (pol > 0) {
					// topology should be linked in polygon
					long iTopStart = topStarts[pol];
					long iTopCurr;
					long iTopNext = iTopStart;
					do {
						iTopCurr = iTopNext;
						iTopNext = (iTopCurr > 0) ? aiFwl[iTopCurr] : aiBwl[-iTopCurr];
					} while (iTopNext != iTopStart);
					// topCurr is segment that links to ts (last segment of pol)
					// let fwl or bwl point to island
					if (iTopCurr > 0) {
						aiFwl[iTopCurr] = i;
					}
					else {
						aiBwl[-iTopCurr] = i;
					}
					// let island fwl point to ts
					if (iTopStart > 0) {
						aiFwl[i] = iTopStart;
					}
					else {
						aiFwl[i] = -iTopStart;
					}
				}
			}
		}

		ITable tblArnAtt = mapArn->attributeTable();
		QString sColMap = QFileInfo(_expression.parm(0).value()).baseName();
		std::vector<QVariant> columnValues = tblArnAtt->column(sColMap);
		ITable tblRasAtt = _inputraster->attributeTable();
		if (tblRasAtt.isValid()) {
			quint32 defCount = tblRasAtt->columnCount();
			for (int idx = 0; idx < defCount; idx++) {
				QString colName = tblRasAtt->columndefinitionRef(idx).name();
				IDomain colDomain = tblRasAtt->columndefinitionRef(idx).datadef().domain();
				_outputfeatures->attributeDefinitionsRef().addColumn(ColumnDefinition(colName, colDomain));
			}
		}

		trq()->inform("Creating Polygons\n");
		initialize(iNrPol);

		std::map<long, std::vector<geos::geom::Geometry*>> polygons;
		geos::geom::GeometryFactory* fact = new geos::geom::GeometryFactory();
		i = 0;
		for (std::map<long, long>::iterator topIter = topStarts.begin(); topIter != topStarts.end(); ++topIter) {
			std::vector<geos::geom::CoordinateSequence*> coords;
			long iRaw = topIter->first;
			long iTopStart = topIter->second;
			long iTopCurr = iTopStart;
			trq()->update(1);
			do {
				if (coords.size() == 0 || !appendCoords(coords[coords.size() - 1], topologySegments[abs(iTopCurr)], iTopCurr > 0)) {
					geos::geom::CoordinateSequence* seg = topologySegments[abs(iTopCurr)].clone();
					if (iTopCurr < 0)
						geos::geom::CoordinateSequence::reverse(seg);
					coords.push_back(seg);
				}
				iTopCurr = (iTopCurr > 0) ? aiFwl[iTopCurr] : aiBwl[-iTopCurr];
			} while (iTopCurr != iTopStart);

			autocorrectCoords(coords);

			iRaw = (iRaw == iUNDEF) ? Ilwis::iUNDEF : (iRaw - 1); // convert back to ilwis4 iraws
			if (iRaw != Ilwis::iUNDEF)
				iRaw = columnValues[iRaw].toInt();

			std::vector<std::pair<geos::geom::LinearRing*, std::vector<geos::geom::Geometry*>*>> polys = makePolys(coords, fact);
			for (std::vector<std::pair<geos::geom::LinearRing*, std::vector<geos::geom::Geometry*>*>>::iterator poly = polys.begin(); poly != polys.end(); ++poly) {
				geos::geom::LinearRing* ring = poly->first;
				std::vector<geos::geom::Geometry*>* holes = poly->second;
				geos::geom::Polygon* gpol(fact->createPolygon(ring, holes)); // takes ownership of both ring and holes pointers
				polygons[iRaw].push_back(gpol);
			}
			++i;
		}
		std::map<long, geos::geom::Geometry*> multiPolygons = makeMultiPolys(polygons, fact);
		for (std::map<long, geos::geom::Geometry*>::iterator mpol = multiPolygons.begin(); mpol != multiPolygons.end(); ++mpol)
		{
			long iRaw = mpol->first;
			if (iRaw == Ilwis::iUNDEF)
				continue;
			SPFeatureI outfeature = _outputfeatures->newFeature(mpol->second);
			if (tblRasAtt.isValid()) {
				for (int col = 0; col < tblRasAtt->columnCount(); ++col) {
					// copy attribute value from inputFeature
					QVariant cellValue;
					cellValue = tblRasAtt->cell(col, iRaw);
					outfeature->setCell(col, cellValue);
				}
			}
			else {
				outfeature->setCell(0, iRaw);
			}
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
	bool ok;
	unsigned int connectivity = _expression.parm(1).value().toUInt(&ok);
	if (!ok || (!(connectivity == 4 || connectivity == 8))) {
		ERROR2(ERR_ILLEGAL_VALUE_2, "parameter value", "connectivity number");
		return sPREPAREFAILED;
	}

	IGeoReference inputgrf = _inputraster->georeference();
	_outputfeatures.prepare(QString(INTERNAL_CATALOG + "/%1").arg(outputName));
	ICoordinateSystem csy = inputgrf->coordinateSystem();
	_outputfeatures->coordinateSystem(csy);
	Envelope env = inputgrf->envelope();
	_outputfeatures->envelope(env);

	_fEightCon = (connectivity == 8);
	if (_expression.parameterCount() == 3) {
		QString choice = _expression.input<QString>(2);
		_fSmooth = choice.toLower() == "yes" || choice.toLower() == "true";
	}
	else
		_fSmooth = true;

	return sPREPARED;
}

quint64 RasterToPolygon::createMetadata()
{
	OperationResource operation({ "ilwis://operations/raster2polygon" });
	operation.setSyntax("raster2polygon(inputraster)");
	operation.setDescription(TR("translates the pixels of a rastercoverage to po;ygons in a featurecoverage"));
	operation.setInParameterCount({ 2, 3 });
	operation.addInParameter(0, itRASTER, TR("input rastercoverage"), TR("input rastercoverage with any domain"));
	operation.addInParameter(1, itUINT8, TR("Connectivity"), TR("Connected cells, may be 4 or 8"));
	operation.addOptionalInParameter(2, itBOOL, TR("Smooth"), TR("Apply smoothing"));
	operation.setOutParameterCount({ 1 });
	operation.addOutParameter(0, itPOLYGON, TR("output polygon coverage"), TR("output polygon coverage with the domain of the input map"));
	operation.setKeywords("polygon,raster, feature");
	mastercatalog()->addItems({ operation });
	return operation.id();
}

