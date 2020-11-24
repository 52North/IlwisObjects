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

#include <cmath>
#if defined(_DEBUG) && defined(SWIG_PYTHON_INTERPRETER_NO_DEBUG)
/* Use debug wrappers with the Python release dll */
# undef _DEBUG
# include <Python.h>
# define _DEBUG
#else
# include <Python.h>
#endif
#include "kernel.h"
#include "ilwisdata.h"
#include "catalog.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "ilwiscontext.h"
#include "runpython.h"
#include "runpythonprogram.h"

using namespace Ilwis;
using namespace PythonScript;

RunPythonProgram::RunPythonProgram()
{

}

RunPythonProgram::RunPythonProgram(quint64 metaid, const Ilwis::OperationExpression &expr) : RunPython(metaid,expr)
{

}

OperationImplementation *RunPythonProgram::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RunPythonProgram(metaid, expr);
}

quint64 RunPythonProgram::createMetadata()
{
    OperationResource operation({"ilwis://operations/runpython"});
    operation.setSyntax("runpython(expression)");
    operation.setLongName("Run a python expression");
    operation.setDescription(TR("runs one or more python statements and returns a string referencing their output"));
    operation.setInParameterCount({1});
    operation.addInParameter(0,itSTRING, TR("python statements"),TR("one or more lines of python code"));

    operation.setOutParameterCount({0,1});
    operation.addOutParameter(0,itSTRING, TR("output"),TR("optional output of the executed python lines") );
    operation.setKeywords("python, script");

    operation.checkAlternateDefinition();
    mastercatalog()->addItems({operation});
    return operation.id();
}

OperationImplementation::State RunPythonProgram::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString pythonfile(_expression.input<QString>(0));
    if ( pythonfile.indexOf("file://") == -1){
        QString path = context()->workingCatalog()->resource().url().toString();
        pythonfile += path + "/" + pythonfile;
    }

    QFile file(pythonfile);
    if ( !file.exists()){
        kernel()->issues()->log(TR("Python file doesnt exist:") + _expression.input<QString>(0));
        return sPREPAREFAILED;
    }
    if ( !file.open(QIODevice::ReadOnly|QIODevice::Text)){
        kernel()->issues()->log(TR("Python file can not be opened:") + _expression.input<QString>(0));
        return sPREPAREFAILED;
    }

    QString filecontent = file.readAll();

    _statements = filecontent;

    return sPREPARED;
}


