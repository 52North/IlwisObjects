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

#ifndef SPREADSHEET_H
#define SPREADSHEET_H

namespace Ilwis {
namespace SpreadSheetConnectors {

class SpreadSheet {
public:
    virtual ~SpreadSheet() {}
    virtual bool openSheet(const QString& spreadsheetPath, bool loading=true) = 0;
    virtual QVariant cellValue(quint32 col, quint32 row) const = 0;
    virtual void cellValue(quint32 col, quint32 row, const QVariant& value, bool newRow=false) {}
    virtual quint32 rowCount() const = 0;
    virtual quint32 columnCount()  = 0;
    virtual IlwisTypes cellValueType(quint32 col, quint32 row) const = 0;
    virtual quint32 sheetCount() const  =0;
    virtual QString sheetName(quint32 index) const = 0;
    virtual bool isValid() const = 0;
    virtual bool isRowValid(quint32 rowIndex) const = 0;
    virtual bool isCellValid(quint32 columnIndex,quint32 rowIndex) const = 0;
    virtual bool isReadOnly() const = 0;
    virtual QString format() const = 0;

    virtual QString storeSheet(const QString& spreadsheetPath) const { return sUNDEF;}



};
}
}

#endif // SPREADSHEET_H
