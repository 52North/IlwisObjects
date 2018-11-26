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
#include "operationmetadata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "commandhandler.h"
#include "operation.h"
#include "script.h"
#include "scriptmodel.h"
#include "mastercatalog.h"
#include "ilwiscontext.h"

using namespace Ilwis;
using namespace Ui;

ScriptModel::ScriptModel(QObject *parent) : ResourceModel()
{

}

ScriptModel::ScriptModel(const Ilwis::IScript& script,QObject *parent) : ResourceModel(script->resource(), parent)
{
    _script = script;
    if ( _displayName == "consolescript.py"){
        _displayName = TR("Python Console");
    }
}

ScriptModel::ScriptModel(const IOperationMetaData &metadata, QObject *parent)
{
    _metadata = metadata;
    if ( hasType(_metadata->extendedType(), itSCRIPT)){
        Resource res(metadata->resource().url(true), itSCRIPT);
        _script.prepare(res);
    }
}
void ScriptModel::store() const
{
   if ( _script.isValid()){
       _script->store();
   }
}

QString ScriptModel::storeAs(const QString &fname)
{
    QString filename = fname;
    if ( fname.indexOf("file://") == -1){
        QFileInfo inf = Ilwis::context()->workingCatalog()->resource().url().toLocalFile();
        if ( inf.path() == ""){
            kernel()->issues()->log(TR("Working catalog is not writable for files ") + Ilwis::context()->workingCatalog()->resource().url().toString() );
            return "";
        }
        filename = inf.absoluteFilePath() + "/" + fname;
        filename = QUrl::fromLocalFile(filename).toString();
    }
    if ( filename.indexOf(".py") == -1)
        filename += ".py";
    Ilwis::Resource res(filename, itSCRIPT);
    Ilwis::mastercatalog()->addItems({res});
    QUrl url(filename);
    QFile file(url.toLocalFile());
    if ( file.open(QFile::WriteOnly|QFile::Text)){
        if ( _script.isValid())
            file.write(_script->text().toLocal8Bit());
        else
            file.write("");
    }
    itemRef() = res;
    _script.prepare(res.id());

    return QString::number(res.id()) ;

}

void ScriptModel::setScriptfromFile(const QString &filename)
{
    Ilwis::Resource res(filename, itSCRIPT);
    itemRef() = res;
    _script.prepare(res.id());
    if ( _displayName == "consolescript.py"){
        _displayName = TR("Python Console");
    }

}

QString ScriptModel::text() const
{
    if ( _script.isValid()){
        return _script->text();
    }
    return "";
}

void ScriptModel::text(const QString &txt)
{
    if ( _script.isValid()){
        _script->text(txt);
    }
}

bool ScriptModel::isDefaultConsole() const
{
    bool ok = TR("Python Console") == _displayName;
    return ok;
}

QString ScriptModel::run(const QString &txt)
{
    try{
        QString line = txt; // double qoutes messes things up in expression parsing so we replace them by single quotes
        line.replace("\"", "'");
        Ilwis::SymbolTable syms;
        Ilwis::ExecutionContext ctx;
        QString statement = QString("runpython(\"%1\")").arg(line);
        auto expr = Ilwis::OperationExpression(statement);
        Ilwis::Operation operation(expr);
        operation->execute(&ctx, syms);
        if ( ctx._results.size() > 0 ){
            QString res = syms.getValue(ctx._results[0]).toString();
            return res;
        }
        return "";

    } catch (const Ilwis::ErrorObject){

    }
    return "";
}
