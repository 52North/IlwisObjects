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

#ifndef COLUMNUNARYOPERATION_H
#define COLUMNUNARYOPERATION_H

namespace Ilwis {
namespace BaseOperations{

class ColumnUnaryOperation : public NumericOperation
{
public:
    ColumnUnaryOperation();
    ColumnUnaryOperation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    State prepare(ExecutionContext *ctx,const SymbolTable&);
private:
    ITable _inputTable;
    ITable _outputTable;
    QString _column1 = sUNDEF;
    QString _column2 = sUNDEF;
    QString _outColumn = sUNDEF;
    QString _inColumn = sUNDEF;


    NumericStatistics::PropertySets _method;
    bool _grouped;
    std::vector<quint32> _groupSize = {1,1,1};

    NumericStatistics::PropertySets toMethod(const QString &nm);

    NEW_OPERATION(ColumnUnaryOperation);

};
}
}

#endif // COLUMNUNARYOPERATION_H
