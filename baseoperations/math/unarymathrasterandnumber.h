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

#ifndef UNARYMATHRASTERANDNUMBER_H
#define UNARYMATHRASTERANDNUMBER_H


namespace Ilwis {
class NumericRange;

namespace BaseOperations{

typedef std::function<double(double)> UnaryFunction;

class UnaryMathRasterAndNumber : public UnaryMath
{
public:
    UnaryMathRasterAndNumber();
    UnaryMathRasterAndNumber(quint64 metaid, const Ilwis::OperationExpression &expr, const QString &outputdom, UnaryFunction fun);

protected:
    static Resource populateMetadata(const QString &item, const QString &longnamem);
    Ilwis::OperationImplementation::State prepare(Ilwis::ExecutionContext *, const Ilwis::SymbolTable &);
    bool execute( ExecutionContext *ctx, SymbolTable& symTable);
    virtual NumericRange *constructRangeFrom(const SPNumericRange &range);

    IRasterCoverage _inputGC;
    IRasterCoverage _outputGC;
    BoundingBox _box;
    OperationType _case;
    double _number;

};
}
}

#endif // UNARYMATHRASTERANDNUMBER_H
