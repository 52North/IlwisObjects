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
#include "domain.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "table.h"
#include "selectabletable.h"
#include "basetable.h"
#include "flattable.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "operationhelper.h"
#include "createtable.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateTable)

CreateTable::CreateTable()
{

}

CreateTable::CreateTable(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool CreateTable::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    IFlatTable tbl;
    Resource res = Resource(itFLATTABLE);
    res.prepare();
    //IOOptions options({"create", true}) ;
    tbl.prepare(res); //, options);
    for (auto item : _columns)
        tbl->addColumn(ColumnDefinition(item.first, item.second));
    ctx->_additionalInfo[INPUTISOUTPUTFLAG] = true;
    if ( tbl.isValid()){
		std::map<quint64, IIlwisObject> doms;
		std::vector<IIlwisObject> objs;
		for (auto item : _columns) {
			doms[item.second->id()] = item.second;
		}
		for (auto obj : doms) {
			objs.push_back(obj.second);
		}
		logOperation(tbl, _expression, objs);
        setOutput(tbl,ctx, symTable);
        return true;
    }

    return false;
}

Ilwis::OperationImplementation *CreateTable::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateTable(metaid, expr);
}

Ilwis::OperationImplementation::State CreateTable::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    

    QStringList columns = _expression.input<QString>(0).split("|");
    for (int i = 0; i < columns.size(); i += 2) {
        QString name = columns[i];
        QString sdomain = columns[i + 1];
        IDomain domain;
        if (!domain.prepare(sdomain)) {
            kernel()->issues()->log(TR("could not find ") + sdomain);
            return sPREPAREFAILED;
        }
        _columns[name] = domain;
    }

    return sPREPARED;
}

quint64 CreateTable::createMetadata()
{
    OperationResource resource({"ilwis://operations/createtable"});
    resource.setLongName("Create Table");
    resource.setSyntax("createtable(columns))");
    resource.setInParameterCount({ 1 });
    resource.addInParameter(0, itSTRING, TR("column definitions"), TR("strings that define a column in name/domain pairs. String are seperated by a '|'"));
    resource.parameterNeedsQuotes(0);
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itTABLE, TR("new table"), TR("The newly created empty table"));
    resource.setKeywords("create, table, workflow");

    mastercatalog()->addItems({resource});
    return resource.id();
}
