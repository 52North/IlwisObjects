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

#ifndef TABLECONNECTOR_H
#define TABLECONNECTOR_H

namespace Ilwis {
namespace Ilwis3{

class RawConverter;

class TableConnector : public Ilwis3Connector
{
public:
    TableConnector(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());
    bool loadMetaData(IlwisObject* data,const IOOptions&);
    bool storeMetaData(Ilwis::IlwisObject *obj, const IOOptions& options = IOOptions());
    QString type() const;
    virtual IlwisObject *create() const;
    bool loadData(IlwisObject *, const IOOptions& options = IOOptions());
    bool storeBinaryData(IlwisObject* obj);

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());
    static bool storeTable(const ITable& tbl);
    void attributeDomain(const QString& attdom);
    void selectedRecords(const std::vector<quint32> &recs);
    QString format() const;
private:
    ColumnDefinition getKeyColumn();
    ColumnDefinition makeColumn(const QString &colName, quint64 index);
    QString valueType2DataType(IlwisTypes ty);
    QString getDomainName(const IDomain &dom, bool& isSystem);

    QHash<QString, RawConverter> _converters;
    QString _attributeDomain;
    std::vector<quint32> _selected;
    QString _primaryKey = sUNDEF;
    bool storeColumns(const Ilwis::Table *tbl, const IOOptions &options);
    QString storeNumericColumn(const Ilwis::ColumnDefinition &def, const QString &colName, QString &domName);
};
}
}

#endif // TABLECONNECTOR_H
