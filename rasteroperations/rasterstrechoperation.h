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

#ifndef RASTERSTRECHOPERATION_H
#define RASTERSTRECHOPERATION_H

namespace Ilwis {
namespace RasterOperations {

class LinearStretchOperation : public OperationImplementation
{
public:
    LinearStretchOperation();
    LinearStretchOperation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();


protected:
    IRasterCoverage _inputRaster;
    IRasterCoverage _outputRaster;
    std::vector<std::pair<double, double>> _limitsFrom;
    std::pair<double, double> _limitsTo = std::pair<double, double>(rUNDEF,rUNDEF);

    bool stretch(IRasterCoverage toStretch);
};

class HistogramEqualizationOperation : public OperationImplementation
{
public:
    HistogramEqualizationOperation();
    HistogramEqualizationOperation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

protected:
    IRasterCoverage _stretchFrom;
    IRasterCoverage _stretchTo;


};

}
}

#endif // RASTERSTRECHOPERATION_H
