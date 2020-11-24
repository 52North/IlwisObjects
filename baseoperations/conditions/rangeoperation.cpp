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
#include "kernel.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "rangeoperation.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(RangeOperation)


Ilwis::OperationImplementation *RangeOperation::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RangeOperation(metaid, expr);
}

RangeOperation::RangeOperation()
{
}

RangeOperation::RangeOperation(quint64 metaid,const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool RangeOperation::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    return true;
}

OperationImplementation::State RangeOperation::prepare(ExecutionContext *ctx,const SymbolTable&)
{
   return sPREPARED;
}


quint64 RangeOperation::createMetadata()
{
    OperationResource operation({"ilwis://operations/range"});
    operation.setLongName("Range");
    operation.setSyntax("range(number|string|band|polygon|line|point|feature|record,container)");
    operation.setDescription(TR("Placeholder operation for use in workflows. It enables the range interface in loops"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itSTRING, TR("RangeOperation variable"),TR("Variable name that indicates the type of value that is used in the iteration"));
    operation.addInParameter(1,itSTRING|itCOVERAGE|itTABLE, TR("Data container"),TR("Any object that describes a set of objects"));
    operation.setOutParameterCount({0});
    operation.setKeywords("tests, workflow");

    operation.checkAlternateDefinition();
    mastercatalog()->addItems({operation});
    return operation.id();
}


