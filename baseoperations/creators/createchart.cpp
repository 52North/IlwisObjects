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
#include "createchart.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateChart)

CreateChart::CreateChart()
{

}

CreateChart::CreateChart(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool CreateChart::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

  

    return false;
}

Ilwis::OperationImplementation *CreateChart::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateChart(metaid, expr);
}

Ilwis::OperationImplementation::State CreateChart::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx, st);


  
    return sPREPARED;
}

quint64 CreateChart::createMetadata()
{
    OperationResource resource({ "ilwis://operations/createchart" });
    resource.setLongName("Create Chart");
    resource.setSyntax("createchart(parameters))");
    resource.setInParameterCount({ 1 });
    resource.addInParameter(0, itSTRING, TR("parameters"), TR("strings that define a parameters in key/valeu pairs. String are seperated by a '|'"));
    resource.parameterNeedsQuotes(0);
    resource.setOutParameterCount({ 1 });
    resource.addOutParameter(0, itTABLE, TR("new chart"), TR("The newly created chart"));
    resource.setKeywords("create, table, chart");

    mastercatalog()->addItems({ resource });
    return resource.id();
}
