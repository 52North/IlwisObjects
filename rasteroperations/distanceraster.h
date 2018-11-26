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

#ifndef DISTANCERASTER_H
#define DISTANCERASTER_H


namespace Ilwis {
namespace RasterOperations {

class DistanceRaster : public OperationImplementation
{
public:
    DistanceRaster();
    DistanceRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    ~DistanceRaster();

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);
    static quint64 createMetadata();

    enum CalcDirection{cdForward, cdBackward};

private:
    bool isValueMap();
    bool isItemMap();

    IRasterCoverage _inputRaster;
    IRasterCoverage _inputOptWeightRaster;
    IRasterCoverage _inputThiessenRaster; // optional
    IRasterCoverage _outputRaster;

    bool            _hasWeightRaster;

    void ThiessenMapCalculation();
    void distanceCalculation();

    NEW_OPERATION(DistanceRaster);
    bool setDistanceValue(PixelIterator & iter, PixelIterator & neighbour, Size<> sz, double weight, CalcDirection cd);
    double Min(double val1, double val2);
};

}
}


#endif // DISTANCERASTER_H
