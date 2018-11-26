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
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "domain.h"
#include "table.h"
#include "commandhandler.h"
#include "operation.h"
#include "mastercatalog.h"
#include "operationhelper.h"
#include "uicontextmodel.h"
#include "modelregistry.h"
#include "chartmodel.h"
#include "chartoperation.h"
#include "createchart.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(CreateChart)

CreateChart::CreateChart()
{
}

CreateChart::CreateChart(quint64 metaid, const Ilwis::OperationExpression &expr) : ChartOperation(metaid, expr)
{

}   

bool CreateChart::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED) 
        if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false; 

    quint32 modelid = _chartmodel->createChart(_name, _table, _chartType, _xaxis, _yaxis, _zaxis);

    logOperation(_expression);
    ctx->setOutput(symTable, modelid, "modelid", itNUMBER, Resource());

    return true;
}

Ilwis::OperationImplementation *CreateChart::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateChart(metaid, expr);  
}  

Ilwis::OperationImplementation::State CreateChart::prepare(ExecutionContext *ctx, const SymbolTable &) 
{
    auto CheckAxis = [](const ITable& tbl, const QString& name, ChartModel::Axis atype)->QString {
        if (atype == ChartModel::Axis::AZAXIS && (name == sUNDEF || name == ""))
            return sUNDEF;

        quint32 index = tbl->columnIndex(name);
        if (index == iUNDEF) {
            return sUNDEF;
        }
        return name; 
    };
    if (!getModel()) {
        kernel()->issues()->log(TR("Invalid model id used for chart. Chart can not be added"));
        return sPREPAREFAILED;
    }
    _name = _expression.input<QString>(1);
    OperationHelper::check([&]()->bool { return _table.prepare(_expression.input<QString>(2), itTABLE); },
    { ERR_NO_OBJECT_TYPE_FOR_2,"table", _expression.input<QString>(1)});

    QString chartType = _expression.input<QString>(3).toLower();
    QStringList types = QString("line|spline|area|bar|pie|points|polar|3dline|3dspline|3dbar").split("|");
    if (!types.contains(chartType)) {
        kernel()->issues()->log(TR("Unknown chart type:") + chartType);
        return sPREPAREFAILED;
    }
    _chartType = chartType;

    OperationHelper::check([&]()->bool { return (_xaxis = CheckAxis(_table, _expression.input<QString>(4), ChartModel::Axis::AXAXIS)) != sUNDEF; },
    { ERR_NO_OBJECT_TYPE_FOR_2,"column", _expression.input<QString>(4) });

    OperationHelper::check([&]()->bool { return (_yaxis = CheckAxis(_table, _expression.input<QString>(5), ChartModel::Axis::AYAXIS)) != sUNDEF; },
    { ERR_NO_OBJECT_TYPE_FOR_2,"column", _expression.input<QString>(5) });

    _zaxis = CheckAxis(_table, _expression.input<QString>(6), ChartModel::Axis::AXAXIS);

     return sPREPARED;
}

quint64 CreateChart::createMetadata()
{
    OperationResource operation({ "ilwis://operations/createchart" });
    operation.setSyntax("createchart(modelid, table, charttype=[Line|Spline|Area|Bar|Pie|Points|Polar|3DLine|3DSpline|3Dbar], xaxis,yaxis,zaxis)");
    operation.setDescription(TR("creates a chart of the specified type"));
    operation.setInParameterCount({ 7 });
    operation.addInParameter(0, itINTEGER, TR("Chart model id"), TR("ID of the model to which this series has to be added"));
    operation.addInParameter(1, itSTRING, TR("Chart name"), TR("Name of the chart"));
    operation.addInParameter(2, itTABLE, TR("Table"), TR("Table from which the series will be collected."));
    operation.addInParameter(3, itSTRING, TR("Chart type"), TR("The type of graph"));
    operation.addInParameter(4, itSTRING, TR("Name X axis"), TR("Name of the column used for the xaxis"));
    operation.addInParameter(5, itSTRING, TR("Name Y axis"), TR("Name of the column used for the yaxis")); 
    operation.addInParameter(6, itSTRING, TR("Name Z axis"), TR("Name of the column used for the yaxis. Undefined in case we have 2 dim graph"));
    operation.setOutParameterCount({ 0 });
    operation.setKeywords("table,visualization,chart");

    mastercatalog()->addItems({ operation });
    return operation.id();
}





