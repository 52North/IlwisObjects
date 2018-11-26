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

#include <iostream>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "workingcatalog.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(WorkingCatalog)


WorkingCatalog::WorkingCatalog()
{
}


WorkingCatalog::WorkingCatalog(quint64 metaid, const Ilwis::OperationExpression &expr) :  OperationImplementation(metaid, expr)
{

}

Ilwis::OperationImplementation *WorkingCatalog::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new WorkingCatalog(metaid, expr);
}

bool WorkingCatalog::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
   logOperation( _expression);
   context()->setWorkingCatalog(ICatalog(_location.toString()));
   mastercatalog()->addContainer(_location);

    return true;
}

Ilwis::OperationImplementation::State WorkingCatalog::prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString location =  _expression.input<QString>(0);
    QUrl url( location);
    if ( !url.isValid() || url.isEmpty() || url.isRelative()){
        ERROR2(ERR_ILLEGAL_VALUE_2,"Url",location);
        return sPREPAREFAILED;
    }
    _location = url;
    return sPREPARED;
}

quint64 WorkingCatalog::createMetadata()
{
    OperationResource operation({"ilwis://operations/setworkingcatalog"});
    operation.setLongName(TR("Set Working Catalog"));
    operation.setDescription(TR("The working catalog is the only location were data maybe used without full url as path. It is a convenience system for creating more readable expressions"));
    operation.setSyntax("setworkingcatalog(url)");
    operation.setInParameterCount({1});
    operation.parameterNeedsQuotes(0);
    operation.addInParameter(0,itSTRING , TR("Location working catalog"),TR("The default location were to look for data as an url"));
    operation.setOutParameterCount({0});
    operation.setKeywords("data,workflow");

    mastercatalog()->addItems({operation});
    return operation.id();
}
