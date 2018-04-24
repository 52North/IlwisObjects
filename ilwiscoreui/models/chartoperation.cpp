#include "kernel.h"
#include "ilwisdata.h"
#include "operationmetadata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operation.h"
#include "uicontextmodel.h"
#include "modelregistry.h"
#include "chartmodel.h"
#include "chartoperation.h"

using namespace Ilwis;
using namespace Ui;

ChartOperation::ChartOperation(){
}

ChartOperation::ChartOperation(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr){
}

ChartModel *ChartOperation::chartModel()
{
    quint32 modelid = _expression.input<quint32>(0);
    std::pair<QString, QObject *> model = modelregistry()->getModel(modelid);
    if (model.first != sUNDEF) {
        return dynamic_cast<ChartModel *>(model.second);
    }
    return 0;
}

bool Ilwis::Ui::ChartOperation::getModel()
{
    quint32 id = _expression.input<quint32>(0);
    auto pair = modelregistry()->getModel(id);
    if (pair.first != sUNDEF) {
         return (_chartmodel = dynamic_cast<ChartModel *>(pair.second)) != 0;
    }

    return false;
}

