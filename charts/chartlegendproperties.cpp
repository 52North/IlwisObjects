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
#include "dataseriesmodel.h"
#include "chartoperation.h"
#include "chartlegendproperties.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(ChartLegendProperties)

ChartLegendProperties::ChartLegendProperties()
{

}

ChartLegendProperties::ChartLegendProperties(quint64 metaid, const Ilwis::OperationExpression &expr) : ChartOperation(metaid, expr)
{

}

Ilwis::OperationImplementation *ChartLegendProperties::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ChartLegendProperties(metaid, expr);
}

Ilwis::OperationImplementation::State ChartLegendProperties::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    ChartOperation::prepare(ctx, st);

    if (!getModel()) {
        kernel()->issues()->log(TR("Invalid model id used for chart. Chart can not change legend"));
        return sPREPAREFAILED;
    }

    bool ok;
    _show = _expression.input<bool>(1, ok);
    if (!ok) {
        kernel()->issues()->log(QString(TR("%1 is not a valid flag").arg(_expression.parm(1).value())));
        return sPREPAREFAILED;
    }

    _align = _chartmodel->legendAlignment();    // get current model value for the optional param
    if (_expression.parameterCount() == 3) {
        QString align = _expression.input<QString>(2).toLower();
        if (!ok) {
            kernel()->issues()->log(QString(TR("%1 is not valid").arg(_expression.parm(2).value())));
            return sPREPAREFAILED;
        }
        const std::vector<QString> alignValues{
            "top", "right", "bottom", "left"
        };
        auto it = std::find(alignValues.begin(), alignValues.end(), align);
        if (it == alignValues.end()) {
            kernel()->issues()->log(QString(TR("%1 : unrecognized alignment value").arg(align)));
            return sPREPAREFAILED;
        }

        _align = align;
    }

    return sPREPARED;
}

bool ChartLegendProperties::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    _chartmodel->setLegendVisible(_show);

    _chartmodel->setLegendAlignment(_align);

    return true;
}

quint64 ChartLegendProperties::createMetadata()
{
    OperationResource resource({ "ilwis://operations/ChartLegendProperties" });
    resource.setLongName("Adjust the legend properties on the chart");
    resource.setSyntax("chartseriestype(parameters)");
    resource.setInParameterCount({ 2, 3 });
    resource.addInParameter(0, itINT32, TR("chart ID"), TR("the ID identifying the chart containing the legend."));
    resource.addInParameter(1, itSTRING, TR("visible"), TR("controls visibility of the legend"));
    resource.addOptionalInParameter(2, itSTRING, TR("alignment"), TR("the position of the legend (top, right, left or bottom"));
    resource.setKeywords("chart legend, table, chart, legend");

    mastercatalog()->addItems({ resource });
    return resource.id();
}
