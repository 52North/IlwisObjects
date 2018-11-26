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

#ifndef COLUMNJOIN_H
#define COLUMNJOIN_H
namespace Ilwis {
namespace BaseOperations{
class ColumnJoin : public NumericOperation
{
    public:
        ColumnJoin();
        ColumnJoin(quint64 metaid, const Ilwis::OperationExpression &expr);
        bool execute(ExecutionContext *ctx, SymbolTable& symTable);
        static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);

        void addInsertChangedDataToTempTableStmt(const QString &tmpTable, const ITable table);
        QString columnNamesCommaSeparated(const ITable table) const;
        QString createInsertValueString(QVariant value, const ColumnDefinition &coldef) const;
        QString trimAndRemoveLastCharacter(const QString &string) const;
        static quint64 createMetadata();
        State prepare(ExecutionContext *ctx,const SymbolTable&);

        void createTable(const QString &tmpTable, const ITable table);

        void deleteTable(const QString &tmpTable);
private:
        ITable _baseTable;
        ITable _foreignTable;
        ITable _outputTable;
        ICoverage _inputCoverage;
        ICoverage _outputCoverage;
        std::vector<bool> _retainedBaseTableColumns;
        std::vector<bool> _retainedForeignTableColumns;

        QString _primaryKeyColumn = sUNDEF;
        QString _foreignKeyColumn = sUNDEF;
        NEW_OPERATION(ColumnJoin);
      };
 }
}
#endif // COLUMNJOIN_H
