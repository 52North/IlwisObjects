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

#ifndef FEATURESPACE_H
#define FEATURESPACE_H

#include <unordered_map>

namespace std {

  template <>
  struct hash<Ilwis::Pixel>
  {
    std::size_t operator()(const Ilwis::Pixel& k) const
    {
      return ((hash<int>()(k.x)
               ^ (hash<int>()(k.y) << 1)) >> 1);
    }
  };

}

namespace Ilwis {

class SampleSet;

struct FSInfo {
    FSInfo() : _fsFrequency(0), _selectedFrequency(0), _classRaw(iUNDEF) {}
    FSInfo(Raw clsRaw) : _fsFrequency(0), _selectedFrequency(0), _classRaw(clsRaw){}
    int _fsFrequency;
    int _selectedFrequency;
    Raw _classRaw;
};

class FeatureSpace
{
public:
    FeatureSpace(SampleSet* sampleset, quint32 band1, quint32 band2);

    void addSample(const Ilwis::Pixel &xy, Raw classRaw);
    void compute();
    quint64 id() const;

private:
    std::unordered_map<Pixel, FSInfo> _fsInfo;
    std::unique_ptr<SampleSet> _sampleset;
    quint32 _band1;
    quint32 _band2;
    quint64 _id;
    static quint64 _featureSpaceIds;
};
}

#endif // FEATURESPACE_H
