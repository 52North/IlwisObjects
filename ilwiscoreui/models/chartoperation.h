#ifndef DRAWEROPERATION_H
#define DRAWEROPERATION_H

#include "ilwiscoreui_global.h"

class QStandardItem;

namespace Ilwis {
namespace Ui {
class LayerManager;


class ILWISCOREUISHARED_EXPORT ChartOperation : public OperationImplementation
{
public:
    ChartOperation();
    ChartOperation(quint64 metaid, const Ilwis::OperationExpression &expr);

    ChartModel *chartModel();

protected:
    bool getModel();
    ChartModel *_chartmodel = 0;
    ITable _table;
};
}
}

#endif // DRAWEROPERATION_H
