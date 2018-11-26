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

#ifndef OPERATIONSBYKEYMODEL_H
#define OPERATIONSBYKEYMODEL_H

#include <QObject>
#include "operationcatalogmodel.h"

namespace Ilwis {
namespace Ui {

class ILWISCOREUISHARED_EXPORT OperationsByKeyModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMLOperationList operationsList READ operationsList CONSTANT)
    Q_PROPERTY(int operationCount READ operationCount CONSTANT)
    Q_PROPERTY(QString keyword READ keyword CONSTANT)

public:
    OperationsByKeyModel();
    explicit OperationsByKeyModel(const QString& kword,const std::vector<OperationModel *>& opers, QObject *parent = 0);

    QString keyword() const;
    QMLOperationList operationsList();
    int operationCount() const;

signals:

public slots:

private:
    QString _keyword;
    QList<OperationModel *> _operations;

};
}
}

#endif // OPERATIONSBYKEYMODEL_H
