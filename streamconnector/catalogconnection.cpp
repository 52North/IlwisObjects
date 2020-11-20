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
#include "operation.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "catalogconnection.h"

using namespace Ilwis;
using namespace Stream;

    REGISTER_OPERATION(CatalogConnection)

CatalogConnection::CatalogConnection(quint64 metaid, const Ilwis::OperationExpression &expression) : OperationImplementation(metaid, expression)
{

}

bool CatalogConnection::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    QString expression = QString("http://%1:%2/dataaccess?datasource=data&ilwistype=catalog&service=ilwisobjects").arg(_host).arg(_port);

    if ( _username != "" && _password != ""){
        expression += QString("&username=%1&password=%2").arg(_username).arg(_password);
    }

    QVariant value;
    value.setValue<QString>(expression);
    ctx->setOutput(symTable, value, "connectionstring", itSTRING, Resource());

    return true;
}

Ilwis::OperationImplementation *CatalogConnection::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return  new CatalogConnection(metaid, expr);
}

Ilwis::OperationImplementation::State CatalogConnection::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    _host = _expression.input<QString>(0);
    _port = _expression.input<quint32>(1);
    _username = _expression.input<QString>(2);
    _password = _expression.input<QString>(3);
    if ( _host == "" || _port == 0)
        return sPREPAREFAILED;

    return sPREPARED;
}

quint64 CatalogConnection::createMetadata()
{
    OperationResource operation({"ilwis://operations/ilwisremotecatalog"});
    operation.setLongName("Ilwis Remote Catalog");
    operation.setSyntax("ilwisremotecatalog(host, port[,username, password])");
    operation.setDescription(TR("creates a url to access the catalog of a remote ilwis server"));
    operation.setInParameterCount({2,4});
    operation.addInParameter(0,itSTRING , TR("host address"),TR("identifies the host that is running the ilwis server"));
    operation.addInParameter(1,itPOSITIVEINTEGER , TR("port number"),TR("port used on the remote server"));
    operation.addOptionalInParameter(2,itSTRING , TR("username"),TR("username for authentication on the remote server"));
    operation.addOptionalInParameter(3,itSTRING , TR("password"),TR("password for authentication on the remote server"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itSTRING, TR("connection url"),TR("string that is sufficient to access the remote catalog"));
    operation.setKeywords("service,ilwisremote");

    operation.checkAlternateDefinition();
    operation.checkAlternateDefinition();
    mastercatalog()->addItems({operation});
    return operation.id();
}
