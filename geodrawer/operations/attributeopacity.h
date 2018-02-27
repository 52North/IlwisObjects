#ifndef AttributeOpacity_H
#define AttributeOpacity_H

namespace Ilwis {
namespace Ui {

class LayerModel;

class AttributeOpacity : public DrawerOperation
{
public:
    AttributeOpacity();

    AttributeOpacity(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(AttributeOpacity);

private:
    LayerModel *_layer;
    QString _attribute = sUNDEF;
    double _opacity;
    QString _item = sUNDEF;
};
}
}

#endif // AttributeOpacity_H
