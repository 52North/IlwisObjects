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
#include "addcolumn.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(AddColumn)

AddColumn::AddColumn()
{

}

AddColumn::AddColumn(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool AddColumn::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    if(!_inputTable->addColumn(_columnName, _columnDomain))
        return false;

    ctx->_additionalInfo["outputisinput"] = true;
    setOutput(_inputTable,ctx, symTable);

    return true;

}

Ilwis::OperationImplementation *AddColumn::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new AddColumn(metaid, expr);
}

Ilwis::OperationImplementation::State AddColumn::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    bool ok;
    quint64 ilwisid = _expression.parm(0).value().toULongLong(&ok);
    if (ok) {
        _inputTable.prepare(ilwisid);
    }
    else {
        _inputTable.prepare(_expression.input<QString>(0), itTABLE);
    }
    if (!_inputTable.isValid()) {
        kernel()->issues()->log(QString(ERR_COULD_NOT_LOAD_2) + _expression.input<QString>(0));
        return  sPREPAREFAILED;
    }

    _columnName = _expression.input<QString>(1);
    for(int c = 0; c < _inputTable->columnCount(); ++c){
        if ( _inputTable->columndefinition(c).name() == _columnName ){
            kernel()->issues()->log(TR("No duplicate column or empty names are allowed : ") + _columnName);
            return sPREPAREFAILED;
        }
    }

    OperationHelper::check([&] ()->bool { return _columnDomain.prepare(_expression.input<QString>(2), itDOMAIN); },
    {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

    if (!hasType(_columnDomain->ilwisType(), itITEMDOMAIN | itNUMERICDOMAIN | itTEXTDOMAIN)){
         kernel()->issues()->log(TR("Invalid domain type for a column : ") + TypeHelper::type2name(_columnDomain->ilwisType()));
         return sPREPAREFAILED;
    }

    return sPREPARED;
}

quint64 AddColumn::createMetadata()
{
    OperationResource resource({"ilwis://operations/addcolumn"});
    resource.setLongName("Add Column to Table");
    resource.setSyntax("addcolumn(table,columnname, domain))");
    resource.setInParameterCount({3});
    resource.addInParameter(0, itTABLE|itINT64,TR("base table"), TR("Table to which a new column will be added, can be table or id of an existing table"));
    resource.addInParameter(1, itSTRING,TR("column name"), TR("Name of the column to be added"));
    resource.addInParameter(2, itDOMAIN,TR("column domain"), TR("Domain of the column to be added"));
    resource.setOutParameterCount({1});
    resource.setOutputIsInput(0,0);
    resource.addOutParameter(0, itTABLE, TR("table"), TR("The input table with an extra column"));
    resource.setKeywords("create, table");

    mastercatalog()->addItems({resource});
    return resource.id();
}

