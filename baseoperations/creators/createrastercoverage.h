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

#ifndef CREATERASTERCOVERAGE_H
#define CREATERASTERCOVERAGE_H


namespace Ilwis {
namespace BaseOperations {
class CreateRasterCoverage :  public OperationImplementation
{
public:
    CreateRasterCoverage();

    CreateRasterCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(CreateRasterCoverage);

protected:
    IDomain _domain;
    IDomain _stackDomain;
    IGeoReference _grf;
    std::vector<QString> _stackValueStrings;
    std::vector<double> _stackValueNumbers;
    std::vector<Range *> _ranges;
    std::vector<IRasterCoverage> _bands;
    bool _autoresample = false;
    IRasterCoverage _outputRaster;
    bool _empty = false;

    bool parseStackDefintion(const QString &stackDef);
    bool parseStackDefintionNumericCase(const QString &stackDef);
    bool parseStackDefintionTimeCase(const QString &stackDef);
    QString expandWildCards(const QString &wildmaps);
};

class CreateSimpelRasterCoverage :  public CreateRasterCoverage
{
public:
    CreateSimpelRasterCoverage();

    CreateSimpelRasterCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(CreateSimpelRasterCoverage);
    bool execute(ExecutionContext *ctx, SymbolTable &symTable);
};
}
}
#endif // CREATERASTERCOVERAGE_H
