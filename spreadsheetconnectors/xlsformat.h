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

#ifndef XLSFORMAT_H
#define XLSFORMAT_H

namespace Ilwis {
namespace SpreadSheetConnectors {


class XLSFormat : public SpreadSheet
{
public:
    XLSFormat();
    ~XLSFormat();

   bool openSheet(const QString& spreadsheetPath, bool loading=true);
   QVariant cellValue(quint32 col, quint32 row) const;
   quint32 rowCount() const;
   quint32 columnCount();
   IlwisTypes cellValueType(quint32 col, quint32 row) const;
   quint32 sheetCount() const;
   QString sheetName(quint32 index) const;

   bool isValid() const;
   bool isRowValid(quint32 rowIndex) const;
   bool isCellValid(quint32 columnIndex,quint32 rowIndex) const;
   bool isReadOnly() const;


   QString format() const;
private:
    const void *_handle = 0;
    int _currentSheet = iUNDEF;
};
}
}

#endif // XLSFORMAT_H
