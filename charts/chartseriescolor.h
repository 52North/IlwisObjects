#pragma once

namespace Ilwis {
	namespace Ui {

		class ChartSeriesColor : public ChartOperation
		{
		public:
			ChartSeriesColor();
			ChartSeriesColor(quint64 metaid, const Ilwis::OperationExpression &expr);

			bool execute(ExecutionContext *ctx, SymbolTable& symTable);
			static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression& expr);
			Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

			static quint64 createMetadata();

		private:
			QString _chartType = sUNDEF;
			QString _seriesName = sUNDEF;
			QString _newColor = sUNDEF;

			NEW_OPERATION(ChartSeriesColor);
		};
	}
}