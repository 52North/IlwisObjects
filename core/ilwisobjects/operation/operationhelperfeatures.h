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

#ifndef OPERATIONHELPERFEATURES_H
#define OPERATIONHELPERFEATURES_H

namespace Ilwis {

typedef  std::function<bool(const std::vector<quint32>&)> SubSetAsyncFunc;

class KERNELSHARED_EXPORT OperationHelperFeatures
{
public:
    OperationHelperFeatures();
    static IIlwisObject initialize(const IIlwisObject &inputObject, IlwisTypes tp, quint64 what);
    static int subdivideTasks(ExecutionContext *ctx, const IFeatureCoverage& raster, std::vector<std::vector<quint32> > &subsets);
    template<typename T> static bool execute(ExecutionContext* ctx, T func, IFeatureCoverage& inputFC, IFeatureCoverage& outputFC){
        std::vector<std::vector<quint32>> subsets;

        int cores = OperationHelperFeatures::subdivideTasks(ctx,inputFC, subsets);
        if ( cores == iUNDEF)
            return false;

        std::vector<std::future<bool>> futures(cores);
        bool res = true;

        for(int i =0; i < cores; ++i) {
            futures[i] = std::async(std::launch::async, func, subsets[i]);
        }

        for(int i =0; i < cores; ++i) {
            res &= futures[i].get();
        }

        return true;
    }

    template<typename T> static bool execute(ExecutionContext* ctx, T func, const IFeatureCoverage& inputFC, IFeatureCoverage& outputFC){
        std::vector<std::vector<quint32>> subsets;

        int cores = OperationHelperFeatures::subdivideTasks(ctx,inputFC, subsets);
        if ( cores == iUNDEF)
            return false;

        std::vector<std::future<bool>> futures(cores);
        bool res = true;

        for(int i =0; i < cores; ++i) {
            futures[i] = std::async(std::launch::async, func, subsets[i]);
        }

        for(int i =0; i < cores; ++i) {
            res &= futures[i].get();
        }

        if ( res && outputFC.isValid()) {
            //TODO: better handling for multiple feature types

            for(int i=0; i < outputFC->attributeDefinitions().columnCount(); ++i ){
                ColumnDefinition& def = outputFC->attributeDefinitionsRef().columndefinitionRef(i);
                if ( def.datadef().domain<>()->valueType() & itNUMERIC) {
                    ContainerStatistics<double> stats;
                    std::vector<QVariant> values = outputFC->attributeTable()->column(i);
                    std::vector<double> vec(values.size());
                    for(int i=0; i < vec.size(); ++i) {
                        vec[i] = values[i].toDouble();
                    }
                    stats.calculate(vec.begin(), vec.end());
                    NumericRange *rng = new NumericRange(stats[NumericStatistics::pMIN], stats[NumericStatistics::pMAX], std::pow(10,-stats.significantDigits()));
                    def.datadef().range(rng);
                }
            }
        }
        return true;
    }
};
}

#endif // OPERATIONHELPERFEATURES_H
