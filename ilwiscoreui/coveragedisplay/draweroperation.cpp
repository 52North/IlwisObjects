/*IlwisObjects is a framework for analysis, processing and visualization of remote sensing and gis data
Copyright (C) 2018  52n North

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

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
