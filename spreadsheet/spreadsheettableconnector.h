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

#ifndef SPREADSHEETCONNECTOR_H
#define SPREADSHEETCONNECTOR_H

namespace Ilwis {
class Table;

namespace SpreadSheetConnectors {

class SpreadSheet;

class SpreadSheetTableConnector : public IlwisObjectConnector
{
public:
    SpreadSheetTableConnector(const Ilwis::Resource &resource, bool load, const Ilwis::IOOptions &options=IOOptions());
    SpreadSheetTableConnector(const QString& file);

    bool loadMetaData(IlwisObject* data, const Ilwis::IOOptions &options);
    QString type() const;
    virtual IlwisObject *create() const;
    bool loadData(IlwisObject *, const Ilwis::IOOptions &options = IOOptions());
    bool store(IlwisObject *object, const IOOptions& options = IOOptions());
    QString provider() const;
    const std::unique_ptr<SpreadSheet>& spreadsheet() const;
    bool isValid() const;
    bool isReadOnly() const;

    static bool knownSuffix(const QString& suffix);

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load, const Ilwis::IOOptions &options=IOOptions());

    QString format() const;
private:
    struct ColInfo{
        IlwisTypes _type = itUNKNOWN;
        QString _name = sUNDEF;
    };

    std::unique_ptr<SpreadSheet> _spreadsheet;
    std::vector<bool> _validColumns;
    quint32 _validColumnCount = 0;
    quint32 _headerline = iUNDEF;
    static const std::vector<QString> _suffixes;

    void setColumnDefinitions(Table *tbl, const std::vector<ColInfo> &columnType);
};
}
}

#endif // SPREADSHEETCONNECTOR_H
