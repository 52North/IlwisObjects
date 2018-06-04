#pragma once

namespace Ilwis {
    namespace Ui {

        class CreateChart : public ChartOperation
        {
        public:
            CreateChart();
            CreateChart(quint64 metaid, const Ilwis::OperationExpression &expr);

            bool execute(ExecutionContext *ctx, SymbolTable& symTable);
            static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression& expr);
            Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

            static quint64 createMetadata();

        private:
            QString _xaxis = sUNDEF;
            QString _yaxis = sUNDEF;
            QString _zaxis = sUNDEF;
            QString _chartType = sUNDEF;
            QString _name = sUNDEF;
      

            NEW_OPERATION(CreateChart);
        };
    }
}