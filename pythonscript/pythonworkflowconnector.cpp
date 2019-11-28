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
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "location.h"
#include "workflownode.h"
#include "workflow.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationExpression.h"
#include "generatescriptnode.h"
#include "pythonworkflowconnector.h"

using namespace Ilwis;
using namespace PythonScript;

PythonWorkflowConnector::PythonWorkflowConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) :
    IlwisObjectConnector(resource, load, options)
{

}

bool PythonWorkflowConnector::openTarget() {

    if ( ioOptions().contains("inmemory") && ioOptions()["inmemory"].toBool()){
        _data.clear();
        _data.resize(100000);
        QBuffer *buf=  new QBuffer(&_data);
        buf->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
        _datasource.reset( buf );
        return true;
    }else {
        QString filename = _resource.url(true).toLocalFile();
        QFileInfo inf(filename);
        if ( inf.suffix() != "ilwis"){
            filename = inf.absolutePath() + "/" + inf.fileName();
            QString correctUrl = QUrl::fromLocalFile(filename).toString();
            sourceRef().setUrl(correctUrl);
            sourceRef().setUrl(correctUrl,true);
        }
        QFile *file = new QFile(filename);

        if (file->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
            _datasource.reset(file);
            return true;
        }
    }
    return false;
}

bool PythonWorkflowConnector::store(IlwisObject *obj, const IOOptions &options)
{
    Workflow *workflow = static_cast<Workflow *>(obj);

    //bool inMemory = true;
    if ( !options.contains("expression"))
        return false;

    auto expression = options["expression"].value<OperationExpression>();
    if (!openTarget())
        return false;
    QStringList names, script;

    std::vector<SPWorkFlowNode> nodes = workflow->outputNodes();
    for(SPWorkFlowNode node : nodes ) {
        GenerateScriptNode gsNode(node);
        gsNode.execute(expression, workflow->parmid2order(), script,"");
    }

    QString expr = expression.input<QString>(0); // we dont want the script command, just its tail
    int assignIndex = expr.indexOf("=");
    QString rightTerm = expr.mid(assignIndex + 1);
    QString leftTerm = expr.left(assignIndex);
    QStringList outs = leftTerm.split("},");
    for(auto output : outs){
        QString res ;
        int index = output.lastIndexOf("/");
        if ( index != -1){
            res = output.mid(index + 2);
        }else
            res = output;
        if (!res.endsWith("}")) // due to the nature how split works the last '}' will have been dropped
            res += "}";
        _outputNames.append(res);
    }
    _expression = rightTerm;


    QTextStream stream(_datasource.get());
	stream.setCodec("UTF-8");
    QString txt = script.join("\n");
    int parmIndex;
    int count = 1;
    while((parmIndex = txt.indexOf("?input_")) != -1){
        QString front = txt.left(parmIndex);
        QString back = txt.mid(parmIndex + 8);
        txt = front + "argv[" + QString::number(count) + "]" + back;
        ++count;
    }
    while((parmIndex = txt.indexOf("?output_")) != -1){
        QString front = txt.left(parmIndex);
        QString back = txt.mid(parmIndex + 9);
        txt = front + "argv[" + QString::number(count) + "]" + back;
        ++count;
    }
    stream << txt;
    stream.device()->close();
    return true;

}

QVariant PythonWorkflowConnector::getProperty(const QString &name) const
{
    if ( name == "content"){
        QString s(_data);
        return s;
    }
    return IlwisObjectConnector::getProperty(name);
}

ConnectorInterface *PythonWorkflowConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions &options)
{
    return new PythonWorkflowConnector(resource, load, options);
}

ConnectorInterface *PythonWorkflowConnector::create2(const Ilwis::Resource &resource, bool load, const IOOptions &options)
{
    IOOptions opt = options;
    opt.addOption("inmemory", true);
    return new PythonWorkflowConnector(resource, load, opt);
}

QString PythonWorkflowConnector::type() const
{
    return "Workflow";
}

IlwisObject *PythonWorkflowConnector::create() const
{
    return new Workflow(_resource  );
}

QString PythonWorkflowConnector::provider() const
{
    return "python";
}
