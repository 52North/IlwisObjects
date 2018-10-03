#pragma once

namespace Ilwis {
    namespace Ui {

        class ChartYAxisNumrange : public ChartOperation
        {
        public:
            ChartYAxisNumrange();
            ChartYAxisNumrange(quint64 metaid, const Ilwis::OperationExpression &expr);

            bool execute(ExecutionContext *ctx, SymbolTable& symTable);
            static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression& expr);
            Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

            static quint64 createMetadata();

        private:
            double _low = rUNDEF;
            double _high = rUNDEF;
            bool _fixed = false;
            bool _nice = false;

            NEW_OPERATION(ChartYAxisNumrange);
        };
    }
}