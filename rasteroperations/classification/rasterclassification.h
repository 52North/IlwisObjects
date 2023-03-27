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

#ifndef RASTERCLASSIFICATION_H
#define RASTERCLASSIFICATION_H

namespace Ilwis {
namespace RasterOperations {

class Classifier;

class RasterClassification : public OperationImplementation
{
public:
    RasterClassification();
    RasterClassification(quint64 metaid, const Ilwis::OperationExpression &expr);
    bool execute(ExecutionContext *ctx,SymbolTable& symTable);

protected:
    SampleSet _sampleSet;
    IRasterCoverage _outputRaster;
    std::unique_ptr<Classifier> _classifier;
    QString _type;
    Ilwis::OperationImplementation::State prepare(ExecutionContext *, const SymbolTable &sym);

    static int fillOperationMetadata(OperationResource& operation);
};

class RasterClassificationImpl : public RasterClassification {
public:
    RasterClassificationImpl();
    RasterClassificationImpl(quint64 metaid, const Ilwis::OperationExpression &expr);

    //bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &sym);

    static quint64 createMetadata();

    NEW_OPERATION(RasterClassificationImpl);

private:
};
}
}

#endif // RASTERCLASSIFICATION_H
