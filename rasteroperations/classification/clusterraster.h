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

#ifndef CLUSTERRASTER_H
#define CLUSTERRASTER_H

namespace Ilwis {
namespace RasterOperations {

class ClusterRaster : public OperationImplementation
{
public:
    friend class CompareHistRec;

    ClusterRaster();

    ClusterRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    static const quint8 MAXBANDS = 4;
    static const quint8 MAXCLASSES = 255;

    NEW_OPERATION(ClusterRaster);

private:
    struct HistBandsRec {
      union {
        struct bvals { unsigned int _b1:4, _b2:4, _b3:4, _b4:4; };
        quint16 _comb;
      } _combinations;
      long  _count; // nr of occurrences

      bool operator < (const HistBandsRec& histrec);
    };

    struct BoundRange {
        double bottom, top; // total range
    };

    struct ClusterRecord { // defines cube in feature space
        ClusterRecord() : _firstRecord(-1), _numRecInCube(0), _numPixelsInCube(0) {
            _bandsMin.resize(ClusterRaster::MAXBANDS, 0);
            _bandsMax.resize(ClusterRaster::MAXBANDS, 0);
        }

        double clusterSize(quint32 index){
            return _bandsMax[index] - _bandsMin[index];
        }

        std::vector<double> _bandsMin;
        std::vector<double>_bandsMax; // min/max of feature space
        long _firstRecord;                 // first record in HistRGB that lays in cube
        long _numRecInCube;                // nr. records in HistRGB that lay in cube
        long _numPixelsInCube;                  // total nr. of pixels that lay in cube
    };


    IRasterCoverage _inputRaster;
    IRasterCoverage _outputRaster;
    quint8 _numberOfThematicItems;
    bool _createAttributeTable = false;
    IThematicDomain _thematicDomain;
    int _indexShift;
    int _shift;
    quint16 _reducedValueSpread;
    std::vector<HistBandsRec> _histbands;
    std::vector<std::vector<double>> _aarIndex2Value;
    std::vector<std::vector<double>> _linebuffer;
    BoundRange _abrLookup[MAXBANDS];


    void initFeatureSpaceHistogram(int noOfbands);
    void initStretchLookup();
    long getFSIndex(PixelIterator &iterIn);
    int clusterCalculation(long clusterCombinations, long total,std::vector<ClusterRecord>& newClusterSet);
    void initClusterSet(long total, long clusterCombinations, std::vector<ClusterRecord>& newClusterSet);

};

class CompareHistRec{
public:
    CompareHistRec(ClusterRaster *cr) : app(cr) {}
    bool compareHistRec(const ClusterRaster::HistBandsRec &h1,const ClusterRaster::HistBandsRec &h2, quint32 sortBands);

private:
    ClusterRaster *app;
};
}
}

#endif // CLUSTERRASTER_H
