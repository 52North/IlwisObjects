#include "kernel.h"
#include "ilwisdata.h"
#include "operationmetadata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operation.h"
#include "uicontextmodel.h"
#include "layermanager.h"
#include "modelregistry.h"
#include "draweroperation.h"

using namespace Ilwis;
using namespace Ui;

DrawerOperation::DrawerOperation(){
}

DrawerOperation::DrawerOperation(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr){
}

LayerManager *DrawerOperation::layerManager()
{
    std::pair<QString, QObject *> model = modelregistry()->getModel(_viewid);
    if (model.first != sUNDEF) {
        return dynamic_cast<LayerManager *>(model.second);
    }
    return 0;
}

bool DrawerOperation::getViewId(const QString& sview){
    bool ok;
    _viewid = sview.toULongLong(&ok);
	auto manager = layerManager();
	bool isUndef = isNumericalUndef(_viewid);
    if ( !ok || isUndef || manager == 0){
             kernel()->issues()->log(TR("Illegal viewer identified:") + sview) ;
            return false;
    }
    return true;
}
