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

#ifndef AggregateRaster_H
#define AggregateRaster_H

namespace Ilwis {
namespace RasterOperations {
class AggregateRaster : public OperationImplementation
{
public:
    AggregateRaster();
    AggregateRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();


private:
    quint32 groupSize(int dim=0);
    IIlwisObject _inputObj;
    IIlwisObject _outputObj;
    IGeoReference _targetGrf;
    NumericStatistics::PropertySets _method;
    bool _grouped;
    std::vector<quint32> _groupSize = {1,1,1};

    NumericStatistics::PropertySets toMethod(const QString &nm);
    void executeGrouped(const BoundingBox &inpBox);
    void executeNonGrouped(const BoundingBox &inpBox);
};
}
}

#endif // AggregateRaster_H
