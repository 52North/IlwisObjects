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

#ifndef ODSFORMAT_H
#define ODSFORMAT_H

namespace ods{
class Book;
class Row;
class Sheet;
class Cell;
}
namespace Ilwis {
namespace SpreadSheetConnectors {


class ODSFormat : public SpreadSheet
{
public:
    ODSFormat();
	~ODSFormat();

    bool openSheet(const QString& spreadsheetPath, bool isLoading=true);
    QVariant cellValue(quint32 col, quint32 row) const;
    void cellValue(quint32 col, quint32 row, const QVariant& value, bool newRow = false);
    quint32 rowCountRaw() const;
    quint32 rowCount() const;
    quint32 columnCount();
    IlwisTypes cellValueType(quint32 col, quint32 row) const;
    quint32 sheetCount() const;
    QString sheetName(quint32 index) const;

    bool isValid() const;
    bool isRowValid(quint32 rowIndex) const;
    bool isCellValid(quint32 columnIndex,quint32 rowIndex) const;

    bool isReadOnly() const;

    QString storeSheet(const QString& spreadsheetPath) const ;

    QString format() const;
private:
    ods::Book *_book = 0;
    ods::Sheet *_currentSheet;
    QVector<ods::Row*>* _rows;
    quint32 _columnCount = iUNDEF;

    ods::Cell *getCellInternal(quint32 col, quint32 row) const;
};
}
}

#endif // ODSFORMAT_H
