#pragma once

namespace Ilwis {
    namespace Ui {

        class LayerModel;

        class DrawerInterface;

        class SetActiveAttribute : public DrawerOperation
        {
        public:
            SetActiveAttribute();

            SetActiveAttribute(quint64 metaid, const Ilwis::OperationExpression &expr);

            bool execute(ExecutionContext *ctx, SymbolTable& symTable);
            static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression& expr);
            Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

            static quint64 createMetadata();
        private:
            NEW_OPERATION(SetActiveAttribute);
            LayerModel *_layer = 0;
            QString  _attributeName = sUNDEF;

        };
    }
}
