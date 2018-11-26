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

#ifndef ERROROBJECT_H
#define ERROROBJECT_H

#include "kernel_global.h"

#include <exception>
#include <QString>

namespace Ilwis {

/*!
 * \brief The ErrorObject class base class for all exceptions in Ilwis
 *
 *Exceptions are not widely used in ILwis, there is more a reliance on return values. But were they are used they must be derived from ErrorObject
 *The reason is that there must still be logging to the issuelogger and this can be done through the errorobject
 */
class KERNELSHARED_EXPORT ErrorObject :public std::exception{
public:
    /*!
     * \brief ErrorObject constructor
     * \param message A string message describing the error that occured
     */
    ErrorObject(const QString& message, bool log = true);
    ~ErrorObject() throw();
    /*!
     * \brief message
     * \return returns the message of the exception
     */
    QString message() const;
    virtual const char* what() const throw();

private:
    QString _message;

};

class KERNELSHARED_EXPORT InternalError : public ErrorObject {
public:
    explicit InternalError(const QString& message);
    const char* what() const throw();
};

class KERNELSHARED_EXPORT ScriptError : public ErrorObject {
public:
    explicit ScriptError(const QString& message);
    virtual const char* what() const throw();
};
class KERNELSHARED_EXPORT ScriptSyntaxError : public ScriptError {
public:
    explicit ScriptSyntaxError(const QString& message);
    const char* what() const throw();
};

class KERNELSHARED_EXPORT ScriptExecutionError : public ScriptError {
public:
    explicit ScriptExecutionError(const QString& message);
    const char* what() const throw();
};

class KERNELSHARED_EXPORT FeatureCreationError : public ErrorObject {
public:
    explicit FeatureCreationError(const QString& message);
    const char* what() const throw();
};

class KERNELSHARED_EXPORT GeometryError : public ErrorObject {
public:
    explicit GeometryError(const QString& message);
    const char* what() const throw();
};

class KERNELSHARED_EXPORT OutOfMemoryError : public ErrorObject {
public:
    explicit OutOfMemoryError(const QString& message, bool log = true);
    const char* what() const throw();
};

class KERNELSHARED_EXPORT CheckExpressionError : public ErrorObject {
public:
    explicit CheckExpressionError(const QString& message);
    const char* what() const throw();
};

class KERNELSHARED_EXPORT AbortOperation : public ErrorObject {
public:
    explicit AbortOperation(const QString& message);
    virtual const char* what() const throw();
};

class KERNELSHARED_EXPORT VisualizationError : public ErrorObject {
public:
    explicit VisualizationError(const QString& message);
    virtual const char* what() const throw();
};
}

#endif // ERROROBJECT_H
