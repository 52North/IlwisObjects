#pragma once

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

namespace Ilwis {
	namespace FeatureOperations {

		class CachedRelation
		{
		public:
			CachedRelation() {};
			bool within(geos::geom::Geometry* g1, geos::geom::Geometry* g2) {
				std::map<std::pair<geos::geom::Geometry*, geos::geom::Geometry*>, bool>::iterator item = cachedWithin.find(std::pair<geos::geom::Geometry*, geos::geom::Geometry*>(g1, g2));
				if (item != cachedWithin.end()) {
					return item->second;
				}
				else {
					std::map<std::pair<geos::geom::Geometry*, geos::geom::Geometry*>, bool>::iterator item = cachedWithin.find(std::pair<geos::geom::Geometry*, geos::geom::Geometry*>(g2, g1));
					if (item != cachedWithin.end()) {
						bool within = item->second;
						if (within) { // return false, unless equal
							within = g2->equalsExact(g1); // two equal geometries are always "within" eachother
							cachedWithin[std::pair<geos::geom::Geometry*, geos::geom::Geometry*>(g1, g2)] = within;
							return within;
						}
					}
					bool within = g1->within(g2);
					cachedWithin[std::pair<geos::geom::Geometry*, geos::geom::Geometry*>(g1, g2)] = within;
					return within;
				}
			};
		private:
			std::map<std::pair<geos::geom::Geometry*, geos::geom::Geometry*>, bool> cachedWithin;
		};

		class CoordBuf : public geos::geom::CoordinateArraySequence {
		public:
			int iSize() const { return (int)size(); };
			CoordBuf();
			CoordBuf(int iSize);
			CoordBuf(const CoordinateSequence* seq);
			Coordinate operator[](int i) const;
			Coordinate& operator[](int i);
			void Size(int iBufSize);
			CoordBuf& operator=(const CoordBuf& b);
			CoordBuf& operator=(const CoordinateSequence* b);
		};

		enum DirBound { dbNONE, dbUP, dbDOWN, dbLEFT, dbRIGHT };

		class ChainRec {
		public:
			ChainRec() { iLength = 1; dbCode = dbNONE; }
			ChainRec(DirBound db, long iLen) { iLength = iLen; dbCode = db; }
			DirBound dbCode;
			long iLength;
		};

		class SegBound {
		public:
			long iSegNr;
			bool fBeginSeg, fEndSeg;
			Coordinate crdFrom, crdTo;
			std::vector<ChainRec> dlChain;
			long iLeftRaw, iRightRaw;
			void Insert(DirBound db, long iLength) {
				dlChain.insert(dlChain.begin(), ChainRec(db, iLength));
			}
			void Append(DirBound db, long iLength) {
				dlChain.push_back(ChainRec(db, iLength));
			}
			ChainRec& crFirst() {
				return dlChain.front();
			}
			ChainRec& crLast() {
				return dlChain.back();
			}
		};

		class RasterToPolygon : public OperationImplementation
		{
		public:
			enum LineDirection { ldDOWN = 1, ldUP = 2, ldRIGHT = 4, ldLEFT = 8 };
			RasterToPolygon();
			~RasterToPolygon();
			RasterToPolygon(quint64 metaid, const Ilwis::OperationExpression &expr);

			bool execute(ExecutionContext *ctx, SymbolTable& symTable);
			static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression& expr);
			Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

			static quint64 createMetadata();

		private:
			const long iUNDEF = -2147483647; // Take care! Local override, to make Ilwis3 code work as-is.
			void NewNode(long iLine, long iCol, byte bt);
			void AppendLeftUp(long iLine, long iCol);
			void AppendUp(long iLine, long iCol, byte bt);
			void AppendLeft(long iLine, long iCol, byte bt);
			SegBound* sbNewInBetween(long iCol);
			SegBound* sbNewWithOneEnd(long iLine, long iCol, bool fRight, int& fBegin);
			void EndOfSegment(long iLine, long iCol, SegBound& sb, bool fUp, int& fBegin);
			void StoreSegm(SegBound& sb);
			void StoreSegm(const SegBound& sb, CoordBuf& cBuf, long& iCrd);
			long iNewSegNr();
			void DetLink(DirBound db1, DirBound db2, DirBound db3,
				const std::vector<int>& fSegExist, const std::vector<int>& fBeginSeg,
				const std::vector<SegBound*>& sbSeg);

			bool fFindBoundaries(const Ilwis::IRasterCoverage& mpAreas);

			bool appendCoords(geos::geom::CoordinateSequence*& coordsA, geos::geom::CoordinateSequence& coordsB, bool fForward) const;
			void autocorrectCoords(std::vector<geos::geom::CoordinateSequence*>& coords) const;
			std::vector<std::pair<geos::geom::LinearRing*, std::vector<geos::geom::Geometry*>*>> makePolys(std::vector<geos::geom::CoordinateSequence*>& coords, geos::geom::GeometryFactory* fact) const;
			std::map<long, geos::geom::Geometry*> makeMultiPolys(std::map<long, std::vector<geos::geom::Geometry*>> polygons, geos::geom::GeometryFactory* fact);

			IFeatureCoverage _outputfeatures;
			IRasterCoverage _inputraster;

			LongBufExt iBufInp;     // rasvec 1.4 var: line
			LongBufExt iBufInpPrev; // rasvec 1.4 var: last_line
			ByteBufExt dbBufPrev;   // rasvec 1.4 var: prev
			ByteBufExt dbBufCurr;   // rasvec 1.4 var: cur
			ByteBufExt dbBufNext;   // rasvec 1.4 var: next
			bool _fEightCon, _fSmooth;
			long iSegNr;
			BufExt<SegBound*> sbHoriz, sbVert;  // rasvec 1.4 vars: segline[RIGHT] and segline[DOWN]

			std::vector<long> aiSegNr;
			long iNrPol, iNrSeg;
			std::vector<long> aiFwl, aiBwl;
			std::map<long, CoordBuf> topologySegments;
			std::map<long, std::pair<long, long>> topology;
			std::map<long, long> topStarts;

			NEW_OPERATION(RasterToPolygon);
		};
	}
}
