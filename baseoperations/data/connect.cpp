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
#include "commandhandler.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include  "raster.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "featurecoverage.h"
#include "operation.h"
#include "operationhelper.h"
#include "connect.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(Connect)

Connect::Connect()
{
}

Connect::Connect(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

template<class T> void setVariant(IIlwisObject& obj, QVariant& v){
    IlwisData<T> obj2 = obj.as<T>();
    v.setValue(obj2);
}

bool Connect::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    QVariant v;
    IlwisTypes tp = _object->ilwisType();
    if ( tp == itRASTER)
        setVariant<RasterCoverage>(_object,v);
    else if ( tp == itTABLE)
        setVariant<Table>(_object,v);
    else if ( tp == itFLATTABLE)
        setVariant<FlatTable>(_object,v);
    else if ( tp == itFEATURE)
        setVariant<FeatureCoverage>(_object,v);
    else if ( tp == itDOMAIN)
        setVariant<Domain>(_object,v);
    else if ( tp == itITEMDOMAIN){
        setVariant<Domain>(_object,v);
    }

    logOperation(_object, _expression);
    ctx->setOutput(symTable, v, _object->name(),tp, _object->resource());
    return true;
}

Ilwis::OperationImplementation *Connect::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
        return new Connect(metaid, expr);
}

Ilwis::OperationImplementation::State Connect::prepare(ExecutionContext *ctx, const SymbolTable& st)
{
    OperationImplementation::prepare(ctx,st);
    QString url = _expression.parm(0).value();
    if (!_object.prepare(url)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,url,"");
        return sPREPAREFAILED;
    }
    return sPREPARED;
}

quint64 Connect::createMetadata()
{

    OperationResource operation({"ilwis://operations/connect"});
    operation.setSyntax("connect(url)");
    operation.setDescription(TR("opens an existing datasource"));
    operation.setInParameterCount({1});
    operation.addInParameter(0,itILWISOBJECT , TR("input url"),TR("The object will opened and is available in the workflow"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itILWISOBJECT , TR("ilwisobject"),TR("a reference to the opened object"));
    operation.setKeywords("workflow");

    mastercatalog()->addItems({operation});
    return operation.id();

}

