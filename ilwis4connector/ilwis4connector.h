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

#ifndef Ilwis4Connector_H
#define Ilwis4Connector_H

#include <QNetworkAccessManager>
#include <QJsonValue> 

namespace Ilwis {
	class DataDefinition;

namespace Ilwis4C {


class Ilwis4Connector : public IlwisObjectConnector
{
public:
    Ilwis4Connector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    virtual ~Ilwis4Connector();

    bool loadData(IlwisObject *data, const IOOptions &options);

    QString provider() const;
     bool dataIsLoaded() const;
    QString format() const;
    static ConnectorInterface *create(const Resource &resource, bool load, const IOOptions &options);
protected:
	static bool store(IlwisObject *obj, const IOOptions& options, QJsonObject& jroot);
	static void store(const QString& v, IlwisTypes valueType, QJsonObject& jobj, const IOOptions &options);
	static int loadMetaData(IlwisObject* object, const IOOptions&, const QJsonValue& jvalue);
	static bool loadMetaData(const QJsonObject& jvalue, IlwisTypes tp, QString& v);
	static void storeDataDef(const DataDefinition& def, QJsonObject& obj) ;
	static void loadDataDef(DataDefinition& def, QJsonObject& jdatadef);
	void flush(const IlwisObject *obj, const QJsonArray& jobjects);
	static QString toString(const QJsonValue& value, const QString& key) {
		QJsonValue v = value[key];
		if (v == QJsonValue::Undefined)
			return sUNDEF;
		return v.toString();
	}
	static double toDouble(const QJsonValue& value, const QString& key) {
		QJsonValue v = value[key];
		if (v == QJsonValue::Undefined || !v.isDouble())
			return rUNDEF;
		return v.toDouble();
	}

	static bool toBool(const QJsonValue& value, const QString& key) {
		QJsonValue v = value[key];
		if (v == QJsonValue::Undefined || !v.isBool())
			return bUNDEF;
		return v.toBool();
	}

	int _version = 0;
protected:
	QString _datafile;
private:
    IlwisObject *create() const;



};

}
}

#endif // Ilwis4Connector_H
