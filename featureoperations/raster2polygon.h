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

		class RasterToPolygon : public OperationImplementation
		{
		public:
			RasterToPolygon();
			~RasterToPolygon();
			RasterToPolygon(quint64 metaid, const Ilwis::OperationExpression &expr);

			bool execute(ExecutionContext *ctx, SymbolTable& symTable);
			static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression& expr);
			Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

			static quint64 createMetadata();

		private:
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
				std::list<ChainRec> dlChain;
				long iLeftRaw, iRightRaw;
				//long iForw, iBackw;
				void Insert(DirBound db, long iLength) {
					dlChain.push_front(ChainRec(db, iLength));
				}
				void Append(DirBound db, long iLength) {
					dlChain.push_back(ChainRec(db, iLength));
				}
				ChainRec& front() {
					return dlChain.front();
				}
				ChainRec& back() {
					return dlChain.back();
				}
			};


			IFeatureCoverage _outputfeatures;
			IRasterCoverage _inputraster;

			void fillLineInfo(const std::vector<double>& inputLine, const std::vector<double>& inputLinePrev, int lineSize, std::vector< DirBound>& dirBoundsCurrent, std::vector< DirBound>& dirBoundsNext) const;
			byte setPixelFlag(const std::vector< DirBound>& dirBoundsPrev, const std::vector< DirBound>& dirBoundsCurrent, const std::vector< DirBound>& dirBoundsNext, int x) const;
			void handleNodeCases(byte PixelFlag, const std::vector<double>& inputLine, const std::vector<double>& inputLinePrev, std::vector<SegBound *>& segBoundsHoriz, std::vector<SegBound *>& segBoundsVert) const;

			NEW_OPERATION(RasterToPolygon);
		};
	}
}
