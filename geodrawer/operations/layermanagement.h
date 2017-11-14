#ifndef LAYERMANAGEMENT_H
#define LAYERMANAGEMENT_H

namespace Ilwis {
namespace Ui {
class LayerManagement : public DrawerOperation
{
public:
    LayerManagement();
    ~LayerManagement();

    LayerManagement(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(LayerManagement);

private:
    int _index;
    QString _command;
    LayerManager::LayerMovement _action;
    LayerModel *_layer;

};
}
}

#endif // LAYERMANAGEMENT_H
