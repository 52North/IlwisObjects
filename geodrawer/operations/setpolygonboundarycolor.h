#ifndef SETPOLYGONBOUNDARYCOLOR_H
#define SETPOLYGONBOUNDARYCOLOR_H


namespace Ilwis {
namespace Ui {


class SetPolygonBoundaryColor : public DrawerOperation
{
public:
    SetPolygonBoundaryColor();
    ~SetPolygonBoundaryColor();

    SetPolygonBoundaryColor(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(SetPolygonBoundaryColor);

private:
    QColor _boundaryColor = true;
    LayerModel  *_layer;
};
}
}

#endif // SETPOLYGONBOUNDARYCOLOR_H
