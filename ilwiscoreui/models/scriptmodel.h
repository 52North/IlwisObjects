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

#ifndef SCRIPTMODEL_H
#define SCRIPTMODEL_H

#include <QObject>
#include "kernel.h"
#include "ilwisdata.h"
#include "resourcemodel.h"
#include "operationmetadata.h"
#include "script.h"

namespace Ilwis {
namespace Ui {
class ILWISCOREUISHARED_EXPORT ScriptModel : public ResourceModel
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text WRITE text NOTIFY textChanged)
    Q_PROPERTY(bool isDefaultConsole READ isDefaultConsole CONSTANT)
public:
    explicit ScriptModel(QObject *parent = 0);
    ScriptModel(const Ilwis::IScript &script, QObject *parent);
    ScriptModel(const Ilwis::IOperationMetaData &metadata, QObject *parent);

    Q_INVOKABLE void store() const;
    Q_INVOKABLE QString storeAs(const QString& file);
    Q_INVOKABLE void setScriptfromFile(const QString& filename);
    QString text() const;
    void text(const QString& txt);
    bool isDefaultConsole() const;



    Q_INVOKABLE QString run(const QString &txt);
signals:
    void textChanged();
public slots:

private:
    Ilwis::IScript _script;
    Ilwis::IOperationMetaData _metadata;
};
}
}
#endif // SCRIPTMODEL_H
