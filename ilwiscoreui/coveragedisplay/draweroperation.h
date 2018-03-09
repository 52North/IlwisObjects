#ifndef DRAWEROPERATION_H
#define DRAWEROPERATION_H

#include "ilwiscoreui_global.h"

class QStandardItem;

namespace Ilwis {
namespace Ui {
class LayerManager;


class ILWISCOREUISHARED_EXPORT DrawerOperation : public OperationImplementation
{
public:
    DrawerOperation();
    DrawerOperation(quint64 metaid, const Ilwis::OperationExpression &expr);

protected:
    quint64 _viewid = i64UNDEF; //uniquely identifies the view
    LayerManager *layerManager();
    QStandardItem *_parentLayer = 0;
    bool getViewId(const QString &sview);
};
}
}

#endif // DRAWEROPERATION_H
