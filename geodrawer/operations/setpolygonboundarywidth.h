#ifndef SETPOLYGONBOUNDARYWIDTH_H
#define SETPOLYGONBOUNDARYWIDTH_H


namespace Ilwis {
namespace Ui {


class SetPolygonBoundaryWidth : public DrawerOperation
{
public:
    SetPolygonBoundaryWidth();
    SetPolygonBoundaryWidth(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(SetPolygonBoundaryWidth);

private:
    double _boundarywidth = true;
    LayerModel *_layer;
};
}
}

#endif // SETPOLYGONBOUNDARYWIDTH_H
