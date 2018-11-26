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

#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include "publicdatabase.h"

namespace Ilwis {
class KERNELSHARED_EXPORT InternalDatabaseConnection
{
public:
    InternalDatabaseConnection();
    InternalDatabaseConnection(const QString& query);
    ~InternalDatabaseConnection();
    bool exec(const QString& query);
    bool next();
    QSqlError lastError() const;
    QVariant value(int i) const;
    QVariant value(const QString& name) const;
    QSqlRecord record() const;
    bool exec();
    bool prepare(const QString& query);
    void bindValue(const QString& placeholder, const QVariant& val,
                   QSql::ParamType type = QSql::In);
    bool isValid() const;
private:
	int _index = 0;
	static std::recursive_mutex _guard;
};
}

#endif // DATABASECONNECTION_H
