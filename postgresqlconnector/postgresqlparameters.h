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

#ifndef POSTGRESQLPARAMETERS_H
#define POSTGRESQLPARAMETERS_H

#include "kernel.h"

namespace Ilwis {
namespace Postgresql {

class PostgresqlParameters
{
public:
    PostgresqlParameters();
    PostgresqlParameters(const IOOptions &options);
    PostgresqlParameters(const QString & user, const QString & pass, const QString & host, const quint32 & port, const QString database = "", const QString schema = "", const QString table = "", const QString column = "", const QString rasterid = "");
    PostgresqlParameters(const QString & str);
    ~PostgresqlParameters();
    QString toString() const;
    QString toStringBasics() const; // only user,pass,host,port,database (which uniquely identifies a db-connection)
    bool valid() const;
    void toIOOptions(IOOptions & options) const;
    QString user() const;
    QString pass() const;
    QString host() const;
    quint32 port() const;
    QString database() const;
    QString schema() const;
    QString table() const;
    QString column() const;
    QString rasterID() const;
    /*
    void setUser(const QString &user);
    void setPass(const QString &pass);
    void setHost(const QString &host);
    void setPort(const quint32 &port);
    */
    void setDatabase(const QString &database);
    void setSchema(const QString &schema);
    void setTable(const QString &table);
    void setColumn(const QString &column);
    void setRasterID(const QString &rasterid);

private:
    void warnEmptyBasics() const;
    QString merge(QString left, QString right, QString delimiter) const;
    void split(QString fileName, QString & left, QString & right, QString delimiter) const;
    void replaceString(QString &str, const QString &search, const QString &replace) const;
    QString _user;
    QString _pass;
    QString _host;
    quint32 _port;
    QString _database;
    QString _schema;
    QString _table;
    QString _column;
    QString _rasterid;
};
}
}

#endif // POSTGRESQLPARAMETERS_H
