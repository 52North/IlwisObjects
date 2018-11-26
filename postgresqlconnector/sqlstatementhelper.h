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

#ifndef SQLSTATEMENTHELPER_H
#define SQLSTATEMENTHELPER_H


#include "postgresqlparameters.h"

namespace Ilwis {

class SPFeatureI;

namespace Postgresql {

class SqlStatementHelper
{
public:
    SqlStatementHelper(const PostgresqlParameters &params);
    ~SqlStatementHelper();

    void addCreateTempTableStmt(const QString &tmpTableName);
    void addInsertChangedDataToTempTableStmt(const QString &tmpTable, const Table *table);
    void addUpdateStmt(const QList<QString> & primaryKeys, const QString &tmpTable, const Table *table);
    void addInsertStmt(const QList<QString> & primaryKeys, const QString &tmpTable, const Table *table);
    void addDeleteStmt(const QString &tmpTable, const Table *table);

    QString columnNamesCommaSeparated(const Table *table) const;
    QString columnValuesCommaSeparated(const SPFeatureI feature) const;
    QString createInsertValueString(QVariant value, const ColumnDefinition &coldef) const;
    QString trimAndRemoveLastCharacter(const QString &string) const;

    QString sql();

private:
    QString createWhereComparingPrimaryKeys(const QList<QString> & primaryKeys, const QString &first, const QString &second) const;
    PostgresqlParameters _params;
    QList<QString> _tmpTables;
    QString _sqlBuilder;
};

}
}

#endif // SQLSTATEMENTHELPER_H
