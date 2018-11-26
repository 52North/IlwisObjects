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

#ifndef PUBLICDATABASE_H
#define PUBLICDATABASE_H

#include <QSqlDatabase>

class QSqlRecord;

namespace Ilwis {
/*!
The PublicDatabase class exposes the internal database of Ilwis to the outside world. The default implementation is an in memory sqllite database
 */
class KERNELSHARED_EXPORT PublicDatabase : public QSqlDatabase
{
public:
    /*!
     PublicDatabase initializes the database. Default sqllite is used but by using this constructor one can overrule it. Care has to be taken that the dialect of sql choosen matches.
     * \param db
     */

    PublicDatabase(const QSqlDatabase& db);
    PublicDatabase();
    /*!
     *  prepare initializes the public database
     *
     *this method loads all the system tables in the database. If an error occurs at this level ilwis will abort an log an appropriate message.
     */
    void prepare();

    /*!
     convenience method to translate a code to record of the database

     Codes are unique for a particular set of classes (e.g. ellipsoids). So if you know the code you can figure out which data
     belongs to it. To simplify getting the data for a code this method exist

     \param code unique identifier for an instance of a class
     \param table table in which the code is the key column
     \param rec the record with the data
     \return bool false if any errors did occur. The issue stack will have the exact error
    */
    bool code2Record(const QString& code, const QString& table, QSqlRecord& rec);
    QString findAlias(const QString& name, const QString& type, const QString& nspace);

	bool exec(int index, const QString& query);
	bool next(int index);
	QSqlError lastError(int index) const;
	QVariant value(int index,int i) const;
	QVariant value(int index, const QString& name) const;
	QSqlRecord record(int index) const;
	bool exec(int index);
	bool prepare(int index, const QString& query);
	void bindValue(int index, const QString& placeholder, const QVariant& val,
		QSql::ParamType type = QSql::In);
	bool isValid(int index) const;
	int freeConnectionIndex() ;
	void freeConnectionIndex(int index);

private:
    void loadPublicTables();
    void insertFile(const QString &filename, QSqlQuery &sqlPublic);
    bool fillEllipsoidRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    bool fillDatumRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    bool fillProjectionRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    bool fillValueDomainRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    bool fillEpsgWithLatLonAxesOrderRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    void insertProj4Epsg(QSqlQuery &sqlPublic);
    bool doQuery(const QString &query, QSqlQuery &sqlPublic);
    bool fillFiltersRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    bool fillRepresentationRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    void insertItemDomains(QSqlQuery &sqlPublic);
    bool fillTeritoryRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    void addRegionallEnvelopes();

	std::vector<QSqlQuery> _connection;
	std::vector<bool> _usedConnections;
};
}

#endif // PUBLICDATABASE_H
