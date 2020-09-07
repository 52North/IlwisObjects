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
			struct PrevLinePoint {
				int _networkLink = iUNDEF;
				int _shadowLink = iUNDEF;

				bool isValid() const { return _networkLink != iUNDEF || _shadowLink != iUNDEF; }

			};

			struct NetworkPoint {
				int _x=iUNDEF;
				int _y=iUNDEF;
				int _links[4]; // 0=left. 1=up; 2=right; 3=down

				bool isValid() const { return _x != iUNDEF && _y != iUNDEF; }
			};

			std::vector<NetworkPoint> _points;
			IFeatureCoverage _outputfeatures;
			IRasterCoverage _inputraster;

			NetworkPoint makeConnection(const Pixel& pCenter, bool isTemp, std::vector<RasterToPolygon::PrevLinePoint>& previousLine);


		

			NEW_OPERATION(RasterToPolygon);
		};
	}
}
