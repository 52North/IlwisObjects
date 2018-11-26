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
    if (id == iUNDEF) {
        _chartmodel = modelregistry()->createChartModel(0); // will get a temporary parent
        return _chartmodel != 0;
    }
    auto pair = modelregistry()->getModel(id);
    if (pair.first != sUNDEF) {
         return (_chartmodel = dynamic_cast<ChartModel *>(pair.second)) != 0;
    }

    return false;
}

