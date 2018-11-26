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

#ifndef CROSSRASTERS_H
#define CROSSRASTERS_H

namespace Ilwis {
namespace RasterOperations {


class CrossRastersBase : public OperationImplementation
{
public:
    enum UndefHandling { uhIgnoreUndef1, uhIgnoreUndef2, uhIgnoreUndef, uhDontCare};
    CrossRastersBase();

    CrossRastersBase(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata(Ilwis::OperationResource &operation);

protected:
    IRasterCoverage _inputRaster1;
    IRasterCoverage _inputRaster2;
    ITable _outputTable;
    IRasterCoverage _outputRaster;
    INamedIdDomain _crossDomain;
    UndefHandling _undefhandling;

    bool crossNoRaster(const BoundingBox& box);
    bool crossWithRaster(const  BoundingBox& box);
    QString determineCrossId(double v1, double v2) const;
    void checkUndef(double &v1, double &v2);
};

class CrossRastersWithRasterOutput : public CrossRastersBase
{
public:
   CrossRastersWithRasterOutput(quint64 metaid, const Ilwis::OperationExpression &expr);

   static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

   static quint64 createMetadata();

    NEW_OPERATION(CrossRastersWithRasterOutput);
};

class CrossRasters : public CrossRastersBase
{
public:
    CrossRasters(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);

    static quint64 createMetadata();

    NEW_OPERATION(CrossRasters);
};
}
}

#endif // CROSSRASTERS_H
