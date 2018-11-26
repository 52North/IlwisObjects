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
#include "internaldatabaseconnection.h"
#include "internaldatabasemodel.h"

using namespace Ilwis;

InternalDatabaseModel::InternalDatabaseModel(QObject *parent) : QObject(parent)
{

}

QVariantList InternalDatabaseModel::query(const QString &query)
{
    InternalDatabaseConnection conn(query);

    QVariantList result;
    int count = 0;
    if (conn.exec()){
        while(conn.next()){
            QSqlRecord rec = conn.record();
            QVariantMap fields;
            ++count;
            for(int i = 0; i< rec.count(); ++i){
                fields[rec.fieldName(i)] = rec.value(i);
            }
            result.append(fields);
        }
    }
    return result;

}

