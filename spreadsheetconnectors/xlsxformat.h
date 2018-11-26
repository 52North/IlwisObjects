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

#ifndef XSLXFORMAT_H
#define XSLXFORMAT_H

namespace QXlsx {
class Document;

}

namespace Ilwis {
namespace SpreadSheetConnectors {


class XLSXFormat : public SpreadSheet
{
public:
    XLSXFormat();

    virtual bool openSheet(const QString& spreadsheetPath, bool isLoading=true);
    virtual QVariant cellValue(quint32 col, quint32 row) const;
    virtual void cellValue(quint32 col, quint32 row, const QVariant& value, bool newRow = false);
    virtual quint32 rowCountRaw() const;
    virtual quint32 rowCount() const;
    virtual quint32 columnCount();
    virtual IlwisTypes cellValueType(quint32 col, quint32 row) const;

    virtual bool isValid() const;
    virtual bool isRowValid(quint32 rowIndex) const;
    virtual bool isCellValid(quint32 columnIndex,quint32 rowIndex) const;

    bool isReadOnly() const;

    virtual QString storeSheet(const QString& spreadsheetPath) const ;

    quint32 sheetCount() const;
    QString sheetName(quint32 index) const;
    QString format() const;
private:
    std::unique_ptr<QXlsx::Document> _xlsxdocument;
};
}
}

#endif // XSLXFORMAT_H
