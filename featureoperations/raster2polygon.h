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
			enum LineDirection { ldDOWN = 1, ldUP = 2, ldRIGHT = 4, ldLEFT = 8 };
			RasterToPolygon();
			~RasterToPolygon();
			RasterToPolygon(quint64 metaid, const Ilwis::OperationExpression &expr);

			bool execute(ExecutionContext *ctx, SymbolTable& symTable);
			static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression& expr);
			Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

			static quint64 createMetadata();

		private:
			struct R2PPoint {
				R2PPoint(int x=iUNDEF, int y=iUNDEF) : _x(x), _y(y) {}
				R2PPoint(double x , double y) : _x(x), _y(y) {}
				quint32 _x;
				quint32 _y;
				bool operator != (const R2PPoint& p) const { return _x != p._x || _y != p._y; }
				bool operator == (const R2PPoint& p) const { return _x == p._x && _y == p._y; }
			};
			struct R2PRing {
				std::vector<R2PPoint> _points;
				std::vector<R2PPoint> _pixelInNeighbour;
			};

			enum DirBound { dbNONE, dbUP=1, dbDOWN=4, dbLEFT=8, dbRIGHT=16 };

			IFeatureCoverage _outputfeatures;
			IRasterCoverage _inputraster;
			IRasterCoverage _handledPixels;
			IRasterCoverage _neighbourCount;


			const double NOTHANDLED = PIXVALUEUNDEF;
			const double HANDLED = 0;

			int countNeighbours(BlockIterator& inIter, PIXVALUETYPE refValue);
			void neighbourvalues(PixelIterator& inIter, PixelIterator& handledIterr, std::vector<PIXVALUETYPE>& neighbours);
			bool addLine(const R2PPoint& tail, const R2PPoint& head, R2PRing& ring) const;
			void shifts(const DoubleVector3D& data, BlockIterator& iterIn);

		

			NEW_OPERATION(RasterToPolygon);
		};
	}
}
