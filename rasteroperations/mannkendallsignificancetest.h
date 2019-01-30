#pragma once
#include "core/ilwisobjects/operation/operation.h"

namespace Ilwis {
	struct ExecutionContext;
	class RasterCoverage;
	typedef IlwisData<RasterCoverage> IRasterCoverage;

	namespace RasterOperations {
		typedef std::vector< std::tuple<double, double, qint32> > Limits;

		class MannKendallSignificanceTest : public OperationImplementation
		{
		public:
			MannKendallSignificanceTest();

			MannKendallSignificanceTest(quint64 metaid, const Ilwis::OperationExpression &expr);

			bool execute(ExecutionContext *ctx, SymbolTable& symTable);
			static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression& expr);
			Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);
			static quint64 createMetadata();

			NEW_OPERATION(MannKendallSignificanceTest);

		private:
			IRasterCoverage _inputRaster;
			IRasterCoverage _outputRaster;
			IIntervalDomain _trendDomain;
			Limits _limits;
			int _maxRaw = -1;

			double trendValue(const std::vector<double>& stackColumn, std::vector<int>& ties) const;
			double calcVarS(int n, const std::vector<int>&ties) const;
			double calcZ(int s, double varS) const;

		};
	}
}

