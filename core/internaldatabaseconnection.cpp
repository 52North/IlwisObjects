#include "kernel.h"
#include "publicdatabase.h"
#include "internaldatabaseconnection.h"

using namespace Ilwis;

std::recursive_mutex InternalDatabaseConnection::_guard;

InternalDatabaseConnection::InternalDatabaseConnection()
{
	_guard.lock();
	_index = kernel()->database()->freeConnectionIndex(); 
}

InternalDatabaseConnection::InternalDatabaseConnection(const QString &query)
{
	_guard.lock();
	_index = kernel()->database()->freeConnectionIndex();
    if(!kernel()->database()->exec(_index, query)){
         kernel()->issues()->logSql(lastError());

    }
}

InternalDatabaseConnection::~InternalDatabaseConnection()
{
	kernel()->database()->freeConnectionIndex(_index);
	_guard.unlock();
}

bool InternalDatabaseConnection::exec(const QString &query)
{
     return kernel()->database()->exec(_index, query);
}

bool InternalDatabaseConnection::next()
{
     return kernel()->database()->next(_index);
}

QSqlError InternalDatabaseConnection::lastError() const
{
    return kernel()->database()->lastError(_index);
}

QVariant InternalDatabaseConnection::value(int i) const
{
     return kernel()->database()->value(_index,i);
}

QVariant InternalDatabaseConnection::value(const QString &name) const
{
    return kernel()->database()->value(_index,name);
}

QSqlRecord InternalDatabaseConnection::record() const
{
    return kernel()->database()->record(_index);
}

bool InternalDatabaseConnection::exec()
{
     return kernel()->database()->exec(_index);
}

bool InternalDatabaseConnection::prepare(const QString &query)
{
	return kernel()->database()->prepare(_index, query);
}

void InternalDatabaseConnection::bindValue(const QString &placeholder, const QVariant &val, QSql::ParamType type)
{
    return kernel()->database()->bindValue(_index, placeholder, val, type);
}

bool InternalDatabaseConnection::isValid() const
{
     //  Locker<std::recursive_mutex> lock(_guard);
    return kernel()->database()->isValid(_index);
}
