#ifndef SHOWPOLYGONBOUNDARIES_H
#define SHOWPOLYGONBOUNDARIES_H

namespace Ilwis {
namespace Ui {


class ShowPolygonBoundaries : public DrawerOperation
{
public:
    ShowPolygonBoundaries();
    ShowPolygonBoundaries(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(ShowPolygonBoundaries);

private:
    bool _boundaryVisibility = true;
    LayerModel *_layer;
};
}
}

#endif // SHOWPOLYGONBOUNDARIES_H
