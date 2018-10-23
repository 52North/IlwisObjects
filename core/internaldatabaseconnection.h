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
