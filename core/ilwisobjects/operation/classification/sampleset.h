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

#ifndef SAMPLESET_H
#define SAMPLESET_H

namespace Ilwis {

class SampleStatistics;
class SampleHistogram;
class SampleSum;
class SampleSumXY;
class FeatureSpace;

typedef std::unique_ptr<SampleStatistics> UPSampleStatistics;
typedef std::unique_ptr<FeatureSpace> UPFeatureSpace;

class KERNELSHARED_EXPORT SampleSet : public Identity
{
public:
    SampleSet();
    SampleSet(const Resource& res);
    SampleSet(const SampleSet& sampleset);
    SampleSet(const IRasterCoverage& samplemaps, const IRasterCoverage& samplemap=IRasterCoverage());
    virtual ~SampleSet();
    bool prepare();
    SampleSet& operator=(const SampleSet& sms);

    void sampleRasterSet(const IRasterCoverage& raster);
    void sampleRaster(const IRasterCoverage &raster);
    void thematicDomain(const IThematicDomain& dom);

    UPSampleStatistics& statistics() ;
    const UPSampleStatistics &statistics() const;
    const IRasterCoverage & sampleRaster() const;
    const IRasterCoverage & sampleRasterSet() const;
    const IThematicDomain & thematicDomain() const;

    bool isValid() const;

    Raw addClass(const QString &className);
    void deleteClass(Raw raw);
    Raw mergeClass(Raw raw1, Raw raw2);
    void decrementSums(const Pixel &pixel, Raw targetClass);
    void incrementSums(const Pixel &pixel, Raw targetClass);
    void incrementSampels(Raw newClass);

    void registerFeatureSpace(FeatureSpace *fs);
    void unregisterFeatureSpace(quint64 id);
    SampleSum& smplSum() const { return *_sampleSum.get(); }
    SampleSumXY& smplSumXY() const { return *_sampleSumXY.get(); }

private:
    IRasterCoverage _sampleMap;
    IRasterCoverage _sampleMaps;
    IThematicDomain _sampleDomain;
    std::unique_ptr<SampleStatistics> _sampleStats;
    std::unique_ptr<SampleHistogram>  _sampleHistogram;
    std::unique_ptr<SampleSum> _sampleSum;
    std::unique_ptr<SampleSumXY> _sampleSumXY;

    quint32 _nrOfClasses = 0;

    std::map<quint64, UPFeatureSpace> _featureSpaces;

    void changeSums(const Pixel &pixel, Raw targetClass, int change);
};
}

Q_DECLARE_METATYPE(Ilwis::SampleSet);

#endif // SAMPLESET_H
