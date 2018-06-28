#pragma once

namespace Ilwis {
    namespace Ui {

        class ChartDataseriesType : public ChartOperation
        {
        public:
            ChartDataseriesType();
            ChartDataseriesType(quint64 metaid, const Ilwis::OperationExpression &expr);

            bool execute(ExecutionContext *ctx, SymbolTable& symTable);
            static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression& expr);
            Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

            static quint64 createMetadata();

        private:
            QString _chartType = sUNDEF;
            QString _seriesName = sUNDEF;
            QString _newType = sUNDEF;

            NEW_OPERATION(ChartDataseriesType);
        };
    }
}