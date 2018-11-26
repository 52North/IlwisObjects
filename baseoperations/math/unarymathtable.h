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

#ifndef UNARYMATHTABLE_H
#define UNARYMATHTABLE_H


namespace Ilwis {
namespace BaseOperations{

class UnaryMathTable : public UnaryMath
{
public:
    UnaryMathTable();
    UnaryMathTable(quint64 metaid, const Ilwis::OperationExpression &expr, const QString& outdom,UnaryFunction fun);

protected:
    static Resource populateMetadata(const QString &item, const QString &longname);
    Ilwis::OperationImplementation::State prepare(Ilwis::ExecutionContext *, const Ilwis::SymbolTable &);
    bool execute( ExecutionContext *ctx, SymbolTable& symTable);

    ITable _inputTable;
    ITable _outputTable;
    QString _inColumn;
    QString _outColumn;

};
}
}

#endif // UNARYMATHTABLE_H
