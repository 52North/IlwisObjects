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
#include "AddChartSeries.h" 

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(AddChartSeries)

AddChartSeries::AddChartSeries()
{

}

AddChartSeries::AddChartSeries(quint64 metaid, const Ilwis::OperationExpression &expr) : ChartOperation(metaid, expr)
{

}

Ilwis::OperationImplementation::State AddChartSeries::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	ChartOperation::prepare(ctx, st);

	if (!getModel()) {
		kernel()->issues()->log(TR("Invalid model id used for chart. Chart can not be added"));
		return sPREPAREFAILED;
	}
    OperationHelper::check([&]()->bool { return _inputTable.prepare(_expression.input<QString>(1), itTABLE); },
    { ERR_COULD_NOT_LOAD_2,_expression.input<QString>(1), "" });


	OperationHelper::check(
		[&]()->bool { return _chartmodel->isValidSeries(_inputTable, _expression.input<QString>(2)); },
		{ ERR_ILLEGALE_OPERATION2, _expression.input<QString>(2), "chart" }
	);

	QString cols = _expression.input<QString>(3);
	auto columns = cols.split("|");
	for (int i = 0; i < columns.size(); ++i) {
		QString col = columns[i];
		if (col.indexOf("-") == 0) // columns that use right side axis use '-' as prefix
			col = col.mid(1);
		OperationHelper::check(
			[&]()->bool { return _chartmodel->isValidSeries(_inputTable, col); },
			{ ERR_ILLEGALE_OPERATION2, _expression.input<QString>(3), "chart" }
		);
		_columnYs.push_back(columns[i]);
	}

	QStringList ex = _expression.input<QString>(4).split("|");
	for (auto kvp : ex) {
		QStringList parts = kvp.split("=");
		if (parts.size() == 2) {
			_extras[parts[0]] = parts[1];
		}
	}
	_columnX = _expression.input<QString>(2);

	return sPREPARED;
} 

bool AddChartSeries::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED) 
			return false;

	if (!_extras.contains("color")) {
		QColor clr = _chartmodel->newColor();
		_extras["color"] = clr;
	}
	for(auto colName : _columnYs)
		_chartmodel->insertDataSeries(_inputTable, _chartmodel->seriesCount(), _columnX, colName, sUNDEF, _extras); 

	_chartmodel->fillTableData();
	_chartmodel->updateSeriesChanged();

	logOperation(_expression);   

	return true;    
}

Ilwis::OperationImplementation *AddChartSeries::create(quint64 metaid, const Ilwis::OperationExpression &expr)  
{
	return new AddChartSeries(metaid, expr); 
}

quint64 AddChartSeries::createMetadata()
{
	OperationResource resource({ "ilwis://operations/addchartdata" });
	resource.setLongName("Add Chart Data Series");
	resource.setSyntax("addchartdata(Chart ID, table, X Column, Y Column, extras))");
	resource.setInParameterCount({ 5 });
	resource.addInParameter(0, itINT32, TR("Chart ID"), TR("The ID identifying the chart to which the series needs to be added. "));
    resource.addInParameter(1, itTABLE, TR("Input table"), TR("Table from which the data comes"));
	resource.addInParameter(2, itSTRING, TR("X Column"), TR("The column to use for the X-axis"));
	resource.addInParameter(3, itSTRING, TR("Y Column"), TR("The column to use for the Y-axis"));
	resource.addInParameter(4, itSTRING, TR("Additional Parameters"), TR("key value pairs of pairs seperated by '|' for special parameters; usually not used"));
	resource.setKeywords("chart series, table, chart");

	mastercatalog()->addItems({ resource });
	return resource.id();
}
