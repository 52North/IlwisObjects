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

#ifndef SAMPLESTATISTICS_H
#define SAMPLESTATISTICS_H

namespace Ilwis {

class RasterCoverage;
class ThematicItem;

typedef IlwisData<RasterCoverage> IRasterCoverage;
typedef IlwisData<ItemDomain<ThematicItem>> IThematicDomain;

const int MAXCLASS{250};

struct SampleCell {
    enum Marker{mMEAN=0, mSTANDARDDEV=1};

    SampleCell() : _classMean(0), _classStd(0){}
    double _classMean;
    double _classStd;
};

class SampleStats {
public:
    SampleStats() : _nrOfBands(0) {}
    virtual void prepare(IThematicDomain thematicDomain, quint32 nrOfBands) {}
    virtual void addClass(Raw key) = 0;
    virtual void delClass(Raw key) = 0;
    virtual void resetClass(Raw key) = 0;
    virtual SampleStats *clone() const = 0;
    template<typename T> T* copy() const{
        return static_cast<T *> (clone());
    }
    virtual void mergeClass(Raw key1, Raw key2) {}
protected:
    quint32 _nrOfBands;
    IThematicDomain _thematicDomain;
};

class SampleHistogram : public SampleStats{
public:
    void prepare(IThematicDomain thematicDomain, const IRasterCoverage &raster);
    void addClass(Raw key);
    void delClass(Raw key);
    void resetClass(Raw key);
    void mergeClass(Raw key1, Raw key2);

    bool exists(Raw key) const;
    SampleStats *clone() const;

    quint32 &at(Raw key, quint32 band, quint32 value);

private:
    std::vector<std::vector<std::vector<quint32>>> _hist;
    IRasterCoverage _raster;
    std::vector<double> _minValuesperBand;
};

class SampleSum : public SampleStats{
public:
    void prepare(IThematicDomain thematicDomain, quint32 nrOfBands);
    void addClass(Raw key);
    void delClass(Raw key);
    void resetClass(Raw key);
    void mergeClass(Raw key1, Raw key2);
    double pixelInClass(Raw key);

    SampleStats *clone() const;

    quint32& at(Raw raw, quint32 band1);
private:
    std::vector<std::vector<quint32>> _sums;
};

class SampleSumXY : public SampleStats
{
public:
    SampleStats *clone() const;
    void prepare(IThematicDomain thematicDomain, quint32 nrOfBands);
    void addClass(Raw key);
    void delClass(Raw key);
    void resetClass(Raw key);
    void mergeClass(Raw key1, Raw key2);
    quint32& at(Raw key, quint32 band1, quint32 band2);

private:
    std::vector<std::vector<std::vector<quint32>>> _sums;

};

class KERNELSHARED_EXPORT SampleStatistics: public SampleStats
{
public:
    SampleStatistics();

    SampleStats *clone() const;
    void prepare(IThematicDomain thematicDomain, quint32 nrOfBands);
    void addClass(Raw key);
    void delClass(Raw key);
    void resetClass(Raw key);
    void mergeClass(quint32 key1, quint32 key2, std::unique_ptr<SampleSum> &sum, std::unique_ptr<SampleSumXY> &sumXY);
    double& at(Raw key, quint32 band, SampleCell::Marker val);
    double at(Raw key, quint32 band, SampleCell::Marker val) const;

private:
    std::vector<std::vector<SampleCell>> _sampleSpace;

    double &valueAt(Raw key, quint32 band, SampleCell::Marker marker);
};

typedef std::unique_ptr<SampleStatistics> UPSampleStatistics;
}

#endif // SAMPLESTATISTICS_H
