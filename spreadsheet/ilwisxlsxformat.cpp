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
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "errorobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "SpreadSheet.h"
#include "xlsxdocument.h"
#include "xlsxworkbook.h"
#include "ilwisxlsxformat.h"

using namespace Ilwis;
using namespace SpreadSheetConnectors;

XLSXFormat::XLSXFormat()
{
}

bool XLSXFormat::openSheet(const QString &spreadsheetPath, bool isLoading)
{
    QFileInfo xlsxinfo(spreadsheetPath);

    QString sheetName;
    if ( xlsxinfo.suffix() != "xlsx"){
        int index  = xlsxinfo.absoluteFilePath().lastIndexOf("/");
        sheetName = xlsxinfo.absoluteFilePath().mid(index + 1);
        xlsxinfo = QFileInfo(xlsxinfo.absolutePath());
    }
    _xlsxdocument.reset(new QXlsx::Document(xlsxinfo.absoluteFilePath()));
    if ( sheetName == ""){
        if ( isLoading) {
            auto *wb = _xlsxdocument->workbook();
            sheetName = wb->sheet(0)->sheetName();
        }else
            sheetName = "Sheet1";
    }
    _xlsxdocument->selectSheet(sheetName);

    return true;
}

QVariant XLSXFormat::cellValue(quint32 col, quint32 row) const
{
    if (! _xlsxdocument)
            return QVariant();

    QXlsx::Cell *cell = _xlsxdocument->cellAt(row + 1, col + 1);
    if (!cell)
        return QVariant();
    QXlsx::Cell::CellType type = cell->cellType();
    if ( type == QXlsx::Cell::StringType || type == QXlsx::Cell::BooleanType || type == QXlsx::Cell::NumberType)
		return cell->value();
    return QVariant();

}

void XLSXFormat::cellValue(quint32 col, quint32 row, const QVariant &value, bool newRow)
{
    if (! _xlsxdocument)
        return;
    IlwisTypes type = Domain::ilwType(value);
    QXlsx::CellRange range = _xlsxdocument->dimension();
    if ( row >= range.topRight().row()){
        _xlsxdocument->setRowHeight(row + 1, 20);
    }
    if ( col >= range.topRight().column()){
        _xlsxdocument->setColumnWidth(col + 1, 20);
    }
    _xlsxdocument->write(row + 1, col + 1,value);
}

quint32 XLSXFormat::rowCountRaw() const
{
    if (! _xlsxdocument)
        return iUNDEF;
    QXlsx::CellRange range = _xlsxdocument->dimension();

    return range.rowCount();
}

quint32 XLSXFormat::rowCount() const
{
    int rowCount = -1;
    quint32 raw = rowCountRaw();
    for(int row = 0; row < raw; ++row){
        if (! isCellValid(0, row)){
            rowCount = row;
            break;
        }
    }

    return rowCount != -1 ? rowCount : raw;
}

quint32 XLSXFormat::columnCount()
{
    if (! _xlsxdocument)
        return iUNDEF;
    QXlsx::CellRange range = _xlsxdocument->dimension();
    return range.columnCount();
}

IlwisTypes XLSXFormat::cellValueType(quint32 col, quint32 row) const
{
    return Domain::ilwType(cellValue(col, row));
}

bool XLSXFormat::isValid() const
{
    return _xlsxdocument != 0;
}

bool XLSXFormat::isRowValid(quint32 rowIndex) const
{
    return rowIndex < rowCount() && isCellValid(0, rowIndex);
}

bool XLSXFormat::isCellValid(quint32 columnIndex, quint32 rowIndex) const
{
    return cellValueType(columnIndex, rowIndex) != itUNKNOWN;
}

bool XLSXFormat::isReadOnly() const
{
    return false;
}

quint32 XLSXFormat::sheetCount() const{
    if (! _xlsxdocument)
        return iUNDEF;
    auto names = _xlsxdocument->sheetNames();
    return names.size();

}

QString XLSXFormat::sheetName(quint32 index) const
{
    if (! _xlsxdocument)
        return sUNDEF;
    auto names = _xlsxdocument->sheetNames();
    if ( index < names.size())
        return names[index];
    return sUNDEF;
}

QString XLSXFormat::format() const
{
    return "XLSX Spreadsheet";
}

QString XLSXFormat::storeSheet(const QString &spreadsheetPath) const
{
    QFile file(spreadsheetPath);
    if (file.exists() && !file.remove())
        return QString("file %1 is read only").arg(spreadsheetPath);
    if ( !isValid())
        return "not a valid spread sheet";

    return _xlsxdocument->saveAs(&file) ? "" : "save failed";
}
