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

#ifndef TABLEMERGER_H
#define TABLEMERGER_H

namespace Ilwis {
class KERNELSHARED_EXPORT TableMerger
{
public:
    TableMerger();
    ITable mergeMetadataTables(const ITable &tbl1, const ITable &tbl2);
    bool mergeMetadataTables(ITable &tblOut, const ITable &tblIn, const std::vector<QString> &columns);
	bool mergeTableData(const ITable &sourceTable1, ITable &targetTable, int offset) const;
    void mergeTableData(const ITable &sourceTable1,const ITable &sourceTable2, ITable &targetTable, const std::vector<QString>& except=std::vector<QString>()) const;
    bool copyColumns(const ITable &tblSource, ITable &tbltarget, int options=0);
	int simpleCopyColumns(const ITable &tblSource, ITable& tbltarget, const std::vector<QString>& columns) const;
private:
    std::map<QString, RenumberMap> _renumberers;
    std::map<QString, QString> _columnRenames;
    ColumnDefinition mergeColumnDefinitions(const Ilwis::ColumnDefinition &def1, const Ilwis::ColumnDefinition &def2, RenumberMap* renum=0);
};
}

#endif // TABLEMERGER_H
