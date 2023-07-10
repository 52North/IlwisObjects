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

#ifndef TIMESAT_NRS_H
#define TIMESAT_NRS_H

namespace Ilwis {
namespace BaseOperations {
class Timesat : public OperationImplementation
{
public:
    Timesat();
    Timesat(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    IIlwisObject _inputObj;
    IIlwisObject _outputObj;

    // parameters
    int _nb;                    // number of maps in maplist
    std::vector<int> _win;      // array of window sizes for fitting
    int _nptperyear = 36;       // number of maps per year
    double _spikecutoff = 0.5;  // cutoff for spike detection
    bool _forceUpperEnvelope;
    bool _lastIterationLikeTIMESATfit;
    bool _extendWindow;

    bool calcFitWindow(const int i, const int ienvi,
                       const std::vector<double> yfit, const std::vector<bool> wfit,
                       double win_thresh,
                       int org_offset,
                       int& m1, int& m2);
    std::vector<bool> detectSpikes(const std::vector<double> y, std::vector<bool> valid);
    std::vector<double> savgol(std::vector<double> y, std::vector<bool> w);

    NEW_OPERATION(Timesat);
};
}
}

#endif // TIMESAT_NRS_H
