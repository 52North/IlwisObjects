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

#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <QObject>
#include <QVector>
#include <QVariant>
#include <map>
#include "kernel_global.h"
#include "ilwis.h"
#include "symboltable.h"

namespace Ilwis {

class OperationExpression;
class OperationImplementation;
class Parameter;
class Resource;

typedef std::function<OperationImplementation *(quint64 metaid, const OperationExpression&)> CreateOperation;

struct KERNELSHARED_EXPORT ExecutionContext {
    void clear(bool resultsOnly=false);
    ExecutionContext(bool threaded=false);
    bool _silent = false;
    bool _threaded = false;
    bool _useAdditionalParameters = false;
    qint16 _scope=0;
    std::vector<QString> _results;
    std::map<QString, QVariant> _additionalInfo;
    QString _masterGeoref;
    QString _masterCsy;
    std::ostream *_out;
    void setOutput(SymbolTable &tbl, const QVariant &var, const QString &nme, quint64 tp, const Ilwis::Resource &resource, const QString &addInfo=sUNDEF);
    void addOutput(SymbolTable &tbl, const QVariant &var, const QString &nme, quint64 tp, const Resource &resource, const QString &addInfo=sUNDEF);

};

class KERNELSHARED_EXPORT CommandHandler : public QObject
{
    Q_OBJECT
public:
    friend KERNELSHARED_EXPORT CommandHandler* commandhandler();

    explicit CommandHandler(QObject *parent);
    ~CommandHandler();

    bool execute(const QString &command, ExecutionContext *ctx);
    bool execute(const QString &command, ExecutionContext *ctx, SymbolTable& symTable);
    void addOperation(quint64 id, CreateOperation op);
    OperationImplementation *create(const Ilwis::OperationExpression &expr);
    quint64 findOperationId(const OperationExpression &expr) const;
    bool parmIsValid(int index, Parameter parm, std::map<QString, QString> values) const;

private:
    std::map<quint64, CreateOperation> _commands;
    static CommandHandler *_commandHandler;


signals:

public slots:

};

KERNELSHARED_EXPORT CommandHandler* commandhandler();
}

#endif // COMMANDHANDLER_H
