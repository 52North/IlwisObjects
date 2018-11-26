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

#ifndef OPERATIONWORKER_H
#define OPERATIONWORKER_H

#include <QObject>
#include <QThread>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"


namespace Ilwis {

class OperationWorker : public QObject{
    Q_OBJECT
public:
    OperationWorker(const OperationExpression& opExpr);

    static void run(const OperationExpression &expression, const QVariantMap& runparams);

    QVariantMap _runparms;
public slots:
    void process();

signals:
    void finished();
    void operationResults(QVariantList values);
    void sendMessage(const QString& type, const QString& subtype, const QVariantMap& parameters);

private:
   OperationExpression _expression;


};
}

#endif // OPERATIONWORKER_H
