#include "kernel.h"
#include "ilwisdata.h"
#include "operationmetadata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operation.h"
#include "uicontextmodel.h"
#include "draweroperation.h"

using namespace Ilwis;
using namespace Ui;

DrawerOperation::DrawerOperation(){
}

DrawerOperation::DrawerOperation(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr){
}

LayerManager *DrawerOperation::layerManager()
{
    return uicontext()->layermanager(_viewid);
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
