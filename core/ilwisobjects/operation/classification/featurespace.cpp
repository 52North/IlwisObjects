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

#include "kernel.h"
#include "ilwisdata.h"
#include "location.h"
#include "itemdomain.h"
#include "raster.h"
#include "pixeliterator.h"
#include "samplestatistics.h"
#include "domainitem.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "sampleset.h"
#include "featurespace.h"

using namespace Ilwis;

quint64 FeatureSpace::_featureSpaceIds = 0;

FeatureSpace::FeatureSpace(SampleSet *sampleset, quint32 band1, quint32 band2) : _sampleset(sampleset), _band1(band1), _band2(band2)
{
    _id = _featureSpaceIds++;
}

void FeatureSpace::addSample(const Pixel &xy, Raw classRaw)
{
    if ( classRaw == 255){
        _fsInfo[xy]._selectedFrequency++;
    }

    _fsInfo[xy]._fsFrequency++;

    if (  _fsInfo[xy]._fsFrequency == 1){
        _fsInfo[xy]._classRaw = classRaw;
        return;
    }

    Raw originalClass = _fsInfo[xy]._classRaw;
    Raw newClass = classRaw;

    bool ok = _sampleset->statistics()->at(newClass, _band1, SampleCell::mSTANDARDDEV) *
            _sampleset->statistics()->at(newClass, _band2, SampleCell::mSTANDARDDEV) <
            _sampleset->statistics()->at(originalClass, _band1, SampleCell::mSTANDARDDEV) *
            _sampleset->statistics()->at(originalClass, _band2, SampleCell::mSTANDARDDEV);
    if ( ok) {
        _fsInfo[xy] = newClass;
    }
}


void FeatureSpace::compute()
{
    PixelIterator iter(_sampleset->sampleRaster());

    while(iter != iter.end()){
        if ( isNumericalUndef(*iter))
            addSample(iter.position(), *iter);
    }
}

quint64 FeatureSpace::id() const
{
    return _id;
}
