#ifndef CLEARDRAWERS_H
#define CLEARDRAWERS_H


namespace Ilwis {
namespace Ui {

class ClearDrawers : public DrawerOperation
{
public:
    ClearDrawers();
    ClearDrawers(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    ICoverage _coverage;
    QString _drawerCode;
    LayerModel *_parenttLayer = 0;

    NEW_OPERATION(ClearDrawers);
};
}
}

#endif // CLEARDRAWERS_H
