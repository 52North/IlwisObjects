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
#include "errorobject.h"


using namespace Ilwis;

ErrorObject::ErrorObject(const QString &message, bool log): _message(message)
{
    if (log)
        kernel()->issues()->log(message);
}

ErrorObject::~ErrorObject() throw()
{
}

QString ErrorObject::message() const
{
    return _message;
}

const char *ErrorObject::what() const throw()
{
    return "ErrorObject";
}

InternalError::InternalError(const QString &message) : ErrorObject(message)
{
}

const char *InternalError::what() const throw()
{
    return "InternalError";
}

ScriptSyntaxError::ScriptSyntaxError(const QString &message) : ScriptError(message)
{
}

const char *ScriptSyntaxError::what() const throw()
{
    return "ScriptSyntaxError";
}

ScriptExecutionError::ScriptExecutionError(const QString &message) : ScriptError(message)
{
}

const char *ScriptExecutionError::what() const throw()
{
    return "ScriptExecutionError";
}

ScriptError::ScriptError(const QString &message) : ErrorObject(message)
{
}

const char *ScriptError::what() const throw()
{
    return "ScriptError";
}

FeatureCreationError::FeatureCreationError(const QString &message) : ErrorObject(message)
{
}

const char *FeatureCreationError::what() const throw()
{
    return "FeatureCreationError";
}

GeometryError::GeometryError(const QString &message) : ErrorObject(message)
{
}

const char *GeometryError::what() const throw()
{
    return "GeometryError";
}

OutOfMemoryError::OutOfMemoryError(const QString &message, bool log) : ErrorObject(message, log)
{
}

const char *OutOfMemoryError::what() const throw()
{
    return "OutOfMemoryError";
}

CheckExpressionError::CheckExpressionError(const QString &message) : ErrorObject(message)
{
}

const char *CheckExpressionError::what() const throw()
{
    return "CheckExpressionError";
}


AbortOperation::AbortOperation(const QString &message) : ErrorObject(message)
{

}

const char *AbortOperation::what() const throw()
{
    return "AbortOperation";
}


VisualizationError::VisualizationError(const QString &message) : ErrorObject(message)
{
}

const char *VisualizationError::what() const throw()
{
    return "VisualizationError";
}
