#pragma once

namespace Ilwis{
    namespace BaseOperations {
    class CreateChart : public OperationImplementation
    {
    public:
        CreateChart();

        CreateChart(quint64 metaid, const Ilwis::OperationExpression &expr);

        bool execute(ExecutionContext *ctx, SymbolTable& symTable);
        static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
        Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

        static quint64 createMetadata();

        NEW_OPERATION(CreateChart);

    private:
    };
}
}