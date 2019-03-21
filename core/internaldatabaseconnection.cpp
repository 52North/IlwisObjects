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
#include "publicdatabase.h"
#include "internaldatabaseconnection.h"

using namespace Ilwis;

std::recursive_mutex InternalDatabaseConnection::_guard;

InternalDatabaseConnection::InternalDatabaseConnection()
{
	_guard.lock();
	_index = kernel()->database()->freeConnectionIndex(); 
}

InternalDatabaseConnection::InternalDatabaseConnection(int debugN)
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
	closeConnection();
}

void InternalDatabaseConnection::closeConnection() {
	kernel()->database()->freeConnectionIndex(_index);
	if (_index != iUNDEF) {
		_index = iUNDEF;
		_guard.unlock();
	}
}

bool InternalDatabaseConnection::exec(const QString &query)
{
	if ( isValid())
		return kernel()->database()->exec(_index, query);
	warning();
	return false;
}

bool InternalDatabaseConnection::next()
{
	if ( isValid())
		return kernel()->database()->next(_index);
	warning();
	return false;
}

QSqlError InternalDatabaseConnection::lastError() const
{
	if ( isValid())
		return kernel()->database()->lastError(_index);
	warning();
	return QSqlError();
}

QVariant InternalDatabaseConnection::value(int i) const
{
	if ( isValid())
		return kernel()->database()->value(_index,i);
	warning();
	return QVariant();
}

QVariant InternalDatabaseConnection::value(const QString &name) const
{
	if ( isValid())
		return kernel()->database()->value(_index,name);
	warning();
	return QVariant();
}

QSqlRecord InternalDatabaseConnection::record() const
{
	if ( isValid())
		return kernel()->database()->record(_index);
	warning();
	return QSqlRecord();
}

bool InternalDatabaseConnection::exec()
{
	if ( isValid())
		return kernel()->database()->exec(_index);
	warning();
	return false;
}

bool InternalDatabaseConnection::prepare(const QString &query)
{
	if ( isValid())
		return kernel()->database()->prepare(_index, query);
	warning();
	return false;
}

void InternalDatabaseConnection::bindValue(const QString &placeholder, const QVariant &val, QSql::ParamType type)
{
	if ( isValid())
		return kernel()->database()->bindValue(_index, placeholder, val, type);
	warning();
}

bool InternalDatabaseConnection::isValid() const
{
	return _index != iUNDEF;
	/*if ( _index != iUNDEF)
		return kernel()->database()->isValid(_index);
	warning();
	return false;*/
}

void InternalDatabaseConnection::warning() const {
	kernel()->issues()->log(TR("Using closed connection"), IssueObject::itWarning);
}
