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
    ctx->_additionalInfo["outputisinput"] = true;
    if ( tbl.isValid()){
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
