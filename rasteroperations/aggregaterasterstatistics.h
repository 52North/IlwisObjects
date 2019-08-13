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

#include <vector>

#ifndef AggregateRasterStatistics_H
#define AggregateRasterStatistics_H

namespace Ilwis {
	namespace RasterOperations {
		typedef std::function<double(std::vector<double>&)> StatFunction;

		class AggregateRasterStatistics : public OperationImplementation
		{
		public:
			AggregateRasterStatistics();
			AggregateRasterStatistics(quint64 metaid, const Ilwis::OperationExpression &expr);

			bool execute(ExecutionContext *ctx, SymbolTable& symTable);
			static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression& expr);
			Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &) override;

			static quint64 createMetadata();

		protected:
			int _startBand = 0;
			int _endBand = iUNDEF;
			IRasterCoverage _inputRaster;
		private:

			IRasterCoverage _outputRaster;
			QString _operationName;
			StatFunction _statisticsFunction;
			

			NEW_OPERATION(AggregateRasterStatistics);

		};

		class AggregateRasterStatisticsBySubset : AggregateRasterStatistics {
			AggregateRasterStatisticsBySubset();
			AggregateRasterStatisticsBySubset(quint64 metaid, const Ilwis::OperationExpression &expr);

			static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression& expr);
			Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &) override;

			static quint64 createMetadata();

			NEW_OPERATION(AggregateRasterStatisticsBySubset);
		};
	}
}

#endif // AggregateRasterStatistics_H

