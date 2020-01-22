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
#include "chartyaxisnumrange.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(ChartYAxisNumrange)

ChartYAxisNumrange::ChartYAxisNumrange()
{

}

ChartYAxisNumrange::ChartYAxisNumrange(quint64 metaid, const Ilwis::OperationExpression &expr) : ChartOperation(metaid, expr)
{

}

Ilwis::OperationImplementation *ChartYAxisNumrange::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ChartYAxisNumrange(metaid, expr);
}

Ilwis::OperationImplementation::State ChartYAxisNumrange::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    ChartOperation::prepare(ctx, st);

    if (!getModel()) {
        kernel()->issues()->log(TR("Invalid model id used for chart. Chart can not assign new Y-axis range"));
        return sPREPAREFAILED;
    }

	_useLeft = _expression.input<bool>(1);
    bool ok;
    _low = _expression.input<double>(2, ok);
    if (!ok) {
        kernel()->issues()->log(QString(TR("%1 is not a valid number").arg(_expression.parm(1).value())));
        return sPREPAREFAILED;
    }
    _high = _expression.input<double>(3, ok);
    if (!ok) {
        kernel()->issues()->log(QString(TR("%1 is not a valid number").arg(_expression.parm(2).value())));
        return sPREPAREFAILED;
    }

    _fixed = _expression.input<bool>(4, ok);
    if (!ok) {
        kernel()->issues()->log(QString(TR("%1 is not a boolean").arg(_expression.parm(3).value())));
        return sPREPAREFAILED;
    }

    if (_expression.parameterCount() == 5) {
        _nice = _expression.input<bool>(4, ok);
        if (!ok) {
            kernel()->issues()->log(QString(TR("%1 is not a boolean").arg(_expression.parm(4).value())));
            return sPREPAREFAILED;
        }
    }

    return sPREPARED;
}

bool ChartYAxisNumrange::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

	if (_useLeft) {
		_chartmodel->setMinYLeft(_low);
		_chartmodel->setMaxYLeft(_high);
		_chartmodel->setFixedYAxisLeft(_fixed);
		_chartmodel->setNiceNumbersYLeft(_nice);
	}
	else {
		_chartmodel->setMinYRight(_low);
		_chartmodel->setMaxYRight(_high);
		_chartmodel->setFixedYAxisRight(_fixed);
		_chartmodel->setNiceNumbersYRight(_nice);    
	}

    return true;
}

quint64 ChartYAxisNumrange::createMetadata()
{
    OperationResource resource({ "ilwis://operations/chartyaxisnumrange" });
    resource.setLongName("Change the Y-axis range of the chart");
    resource.setSyntax("chartyaxisnumrange(chartid, side=true, min, max, fixedrange[,nice numers])");
    resource.setInParameterCount({ 5, 6 });
    resource.addInParameter(0, itINT32, TR("chart ID"), TR("the ID identifying the chart."));
	resource.addInParameter(1, itBOOL, TR("Left Side"), TR("Which side are you setting in case of multiple y axis"));
    resource.addInParameter(2, itNUMBER, TR("low"), TR("the new lowest value for the Y-axis (specify ? to leave unchanged)"));
    resource.addInParameter(3, itNUMBER, TR("high"), TR("the new highest value for the Y-axis (specify ? to leave unchanged)"));
    resource.addInParameter(4, itBOOL, TR("fixed"), TR("fix the range to the min and max values"));
    resource.addOptionalInParameter(5, itBOOL, TR("nice"), TR("round the axis numbers to 'nice' numbers"));
    resource.setKeywords("chart series, table, chart, Y-axis, range");

    mastercatalog()->addItems({ resource });
    return resource.id();
}
