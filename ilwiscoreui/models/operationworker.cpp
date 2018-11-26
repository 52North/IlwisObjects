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

#include <QCoreApplication>
#include <QThread>
#include "operationworker.h"
#include "raster.h"
#include "table.h"
#include "commandhandler.h"
#include "operation.h"
#include "catalog.h"
#include "operationhelper.h"
#include "ilwiscontext.h"
#include "uicontextmodel.h"

using namespace Ilwis;
using namespace Ui;

OperationWorker::OperationWorker(const OperationExpression &opExpr) :_expression(opExpr)
{
   connect(this, &OperationWorker::sendMessage, kernel(), &Kernel::acceptMessage);
}

void OperationWorker::run(const OperationExpression &expression, const QVariantMap& runparams){
    try {

        uicontext()->showLastGeneratedResult("");
        Operation op(expression);
        SymbolTable tbl;
        ExecutionContext ctx;
        ctx._additionalInfo = runparams.toStdMap();
      
        if(op->execute(&ctx, tbl)){
            if ( ctx._results.size() > 0){
                for(auto resultName : ctx._results){
                    Symbol symbol = tbl.getSymbol(resultName);
                    if ( hasType(symbol._type, itINTEGER)){
                        QVariantList varList = symbol._var.value<QVariantList>();
                        if (varList.size() > 0) {
                            QString num = QString::number(varList[0].toInt());
                            kernel()->issues()->log(num, IssueObject::itRESULT);
                        }
                    }else if ( hasType(symbol._type, itDOUBLE)){
                        QVariantList varList = symbol._var.value<QVariantList>();
                        if (varList.size() > 0) {
                            QString num = QString::number(varList[0].toDouble());
                            kernel()->issues()->log(num, IssueObject::itRESULT);
                        }
                    }  else if ( hasType(symbol._type, itSTRING)){
                        QVariantList varList = symbol._var.value<QVariantList>();
                        if (varList.size() > 0)
                            kernel()->issues()->log(varList[0].toString(), IssueObject::itRESULT);
                    }else if ( hasType(symbol._type, itBOOL)){
                        QVariantList varList = symbol._var.value<QVariantList>();
                        if (varList.size() > 0)
                            kernel()->issues()->log(varList[0].toString(), IssueObject::itRESULT);
                    }
                    if ( hasType(symbol._type, itCOVERAGE | itTABLE) ){
                        IIlwisObject obj = OperationHelper::variant2ilwisobject(symbol._var, symbol._type);
                        if ( obj.isValid()){
                            QString lgr = QString("bigthing.newCatalog('itemid=%1',\'%2\',\'%3\', \'other\')").arg(obj->resource().id()).arg(TypeHelper::type2name(obj->ilwisType())).arg(obj->resource().url().toString());
                            uicontext()->showLastGeneratedResult(lgr);
                        }
                    }
                }
            }

            kernel()->issues()->log(QString(TR("Operation has executed succesfully")), IssueObject::itMessage);
        }else {
            qDebug() << "operation failed";
        }
    }catch(const ErrorObject& ){

    }

}

void OperationWorker::process(){
    QThread *thread = QThread::currentThread();
    QVariant var = thread->property("workingcatalog");
    //QVariant runvar = thread->property("runparameters");
    if ( var.isValid()){
        kernel()->setTLS("workingcatalog", new QVariant(var));
    }
    run(_expression, _runparms);

    if ( _runparms.size() > 0){
        bool ok;
        quint32 id = _runparms["runid"].toUInt(&ok);
        if ( ok){
            QVariantMap mp;
            mp["runid"] = id;
            emit sendMessage("operation","stopped", mp );
        }
    }
    emit finished();
}
