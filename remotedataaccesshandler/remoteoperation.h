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

#ifndef REMOTEOPERATION_H
#define REMOTEOPERATION_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "kernel.h"
#include "ilwisdata.h"
#include "mastercatalog.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"

namespace Ilwis {

class SymbolTable;
class ExecutionContext;

namespace RemoteDataAccess {


class RemoteOperation : public QObject, public OperationImplementation
{
    Q_OBJECT
public:
    RemoteOperation();

    RemoteOperation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

   NEW_OPERATION(RemoteOperation)
protected slots:
       void readReady();
       void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
       void error(QNetworkReply::NetworkError code);
       void finishedData();

private:
   QUrl _operationRequest;
   OperationExpression _operationexpr;
   QByteArray _bytes;
};
}
}

#endif // REMOTEOPERATION_H
