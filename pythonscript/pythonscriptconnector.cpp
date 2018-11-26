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
#include "operationmetadata.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationExpression.h"
#include "script.h"
#include "pythonscriptconnector.h"

using namespace Ilwis;
using namespace PythonScript;

PythonScriptConnector::PythonScriptConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) :
        IlwisObjectConnector(resource, load, options)
{

}

bool PythonScriptConnector::loadMetaData(IlwisObject *obj, const Ilwis::IOOptions &options)
{
    if (!obj)
            return false;
    return true;
}

bool PythonScriptConnector::loadData(IlwisObject* obj, const IOOptions& options) {
    QFile file(_resource.url(true).toLocalFile());
    if (file.open(QFile::ReadOnly | QFile::Text)){
        QString text = file.readAll();
        Script *script = static_cast<Script *>(obj);
        script->text(text);
        _binaryIsLoaded = true;
        file.close();
        return true;
    }
    return false;
}

bool PythonScriptConnector::store(IlwisObject *obj, const IOOptions &options)
{
    if (!obj)
            return false;
    Script *script = static_cast<Script *>(obj);
    QFile file(_resource.url(true).toLocalFile());
    if (file.open(QFile::WriteOnly | QFile::Text)){
        file.write(script->text().toLocal8Bit());
        file.close();
        return true;
    }
    return false;
}

QString PythonScriptConnector::type() const
{
    return "Script";
}

Ilwis::IlwisObject *PythonScriptConnector::create() const
{
    return new Script(_resource);
}

ConnectorInterface *PythonScriptConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions &options)
{
  return new PythonScriptConnector(resource, load, options);
}

QString PythonScriptConnector::provider() const
{
  return "python";
}
