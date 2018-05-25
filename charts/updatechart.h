#pragma once

namespace Ilwis {
    namespace Ui {

        class UpdateChartSeries : public ChartOperation
        {
        public:
            UpdateChartSeries();
            UpdateChartSeries(quint64 metaid, const Ilwis::OperationExpression &expr);

            bool execute(ExecutionContext *ctx, SymbolTable& symTable);
            static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression& expr);
            Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

            static quint64 createMetadata();

        private:
            QString _name = sUNDEF;
            QString _columnX = sUNDEF;
            QString _columnY = sUNDEF;

            NEW_OPERATION(UpdateChartSeries);
        };
    }
}