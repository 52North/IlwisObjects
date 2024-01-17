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

#ifndef ADDRASTERBAND_H
#define ADDRASTERBAND_H


namespace Ilwis {
namespace BaseOperations{
class AddRasterBand  : public OperationImplementation
{
public:
    AddRasterBand();
    AddRasterBand(quint64 metaid, const Ilwis::OperationExpression &expr);
    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    State prepare(ExecutionContext *ctx,const SymbolTable&);

private:
    struct BandMergeInfo{
        BandMergeInfo(const IRasterCoverage& bnd) : _band(bnd){}
        IRasterCoverage _band;
        std::vector<std::pair<int,QString>> _clashes;
    };
    IRasterCoverage _inputRaster;
    IRasterCoverage _outputRaster;
    std::vector<BandMergeInfo> _bands;
    RasterCoverage::MergeOptions _mergeOptions = RasterCoverage::moNONE;
    int _insertionPoint=-1;

    void insertBand(const Ilwis::IRasterCoverage &mergeRaster, const QString& index);

    NEW_OPERATION(AddRasterBand)    ;
    void addBands();
    void makeValid();
    quint32 mapIndexes(const IRasterCoverage &inRC, quint32 in, const IRasterCoverage &outRC);
};
}
}
#endif // ADDRASTERBAND_H
