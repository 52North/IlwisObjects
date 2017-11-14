#ifndef DRAWEROPERATION_H
#define DRAWEROPERATION_H

#include "ilwiscoreui_global.h"

namespace Ilwis {
namespace Ui {
class LayerManager;
class LayerModel;

class ILWISCOREUISHARED_EXPORT DrawerOperation : public OperationImplementation
{
public:
    DrawerOperation();
    DrawerOperation(quint64 metaid, const Ilwis::OperationExpression &expr);

protected:
    quint64 _viewid = i64UNDEF; //uniquely identifies the view
    LayerManager *layerManager();
    LayerModel *_parentLayer = 0;
    bool getViewId(const QString &sview);
};
}
}

#endif // DRAWEROPERATION_H
