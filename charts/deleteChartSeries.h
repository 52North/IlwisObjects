#pragma once

namespace Ilwis {
	namespace Ui {

		class deleteChartSeries : public ChartOperation
		{
		public:
			deleteChartSeries();
			deleteChartSeries(quint64 metaid, const Ilwis::OperationExpression &expr);

			bool execute(ExecutionContext *ctx, SymbolTable& symTable);
			static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression& expr);
			Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

			static quint64 createMetadata();

		private:
			QString _name = sUNDEF;
			QString _columnY = sUNDEF;
            ITable _inputTable;

			NEW_OPERATION(deleteChartSeries);
		};
	}
}