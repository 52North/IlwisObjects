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

#include <unordered_map>
#include "kernel.h"
#include "ilwisdata.h"
#include "itemdomain.h"
#include "thematicitem.h"
#include "location.h"
#include "raster.h"
#include "pixeliterator.h"
#include "classification/samplestatistics.h"
#include "classification/sampleset.h"
#include "classifier.h"

using namespace Ilwis;
using namespace RasterOperations;

Classifier::Classifier(const SampleSet &sampleset) : _sampleset(sampleset)
{
}

//-------------------------------------------------------------------
BoxClassifier::BoxClassifier(double widenFactor, const SampleSet &sampleset) : Classifier(sampleset), _widenFactor(widenFactor){

}

bool BoxClassifier::classify(PixelIterator &iterIn, PixelIterator &iterOut) const
{
    Raw currentClass, raw;
    PixelIterator iterEnd = iterOut.end();
    std::vector<double> zcolumn(sampleset().sampleRasterSet()->size().zsize(), rUNDEF);
    while( iterOut != iterEnd)    {
        for (double& v : zcolumn) {
            v = *iterIn;
            ++iterIn;
        }
        raw = rUNDEF;
        for(auto item : sampleset().thematicDomain() ){
            currentClass = _classSequence.at(item->raw());
            bool classFound = true;
            for(int band = 0; band < zcolumn.size(); ++band){
                classFound = classFound &&
                        (zcolumn[band] < _boxMax.at((quint32)currentClass).at(band) &&
                         zcolumn[band] > _boxMin.at((quint32)currentClass).at(band));
                if (!classFound)
                    break;
            }
            if ( classFound)  {
                raw = currentClass;
                break;
            }
        }
        *iterOut = raw;
        ++iterOut;
    }
    return true;
}

bool BoxClassifier::prepare()
{
    std::map<Raw, double> stdProducts;
    for(auto item : sampleset().thematicDomain())
        stdProducts[item->raw()] = 1; //std::numeric_limits<double>::max();
    stdProducts[iUNDEF] = 1; //std::numeric_limits<double>::max();;
    Raw maxRaw = rUNDEF;
    for(auto item : sampleset().thematicDomain()){
        Raw raw = item->raw();
        maxRaw = Ilwis::max(raw, maxRaw);
        for(int band = 0; band < sampleset().sampleRasterSet()->size().zsize(); ++band){
            stdProducts[raw] *= sampleset().statistics()->at(raw,band, SampleCell::mSTANDARDDEV);
            double mean = sampleset().statistics()->at(raw,band, SampleCell::mMEAN);
            if ( mean > 0 && stdProducts[raw] < EPS10){
                return ERROR3(ERR_NO_INITIALIZED_3, "item",sampleset().name(), TR("needs more samples"));
            }
        }
    }
    _classSequence.resize(maxRaw + 1);
    _boxMax.resize(maxRaw + 1);
    _boxMin.resize(maxRaw + 1);
    Raw smallestClass;
    stdProducts[iUNDEF] = std::numeric_limits<double>::max();
    for(auto item1 : sampleset().thematicDomain()) {
        smallestClass = iUNDEF;
        for(auto item2 : sampleset().thematicDomain()){
            if ( stdProducts[item2->raw()] < stdProducts[smallestClass]) {
                smallestClass = item2->raw();
            }
        }
        int raw = item1->raw();
        _classSequence[(quint32)raw] = smallestClass;
        stdProducts[smallestClass] = std::numeric_limits<double>::max();
    }
    for(auto item : sampleset().thematicDomain()) {
        _boxMax[(quint32)item->raw()].clear();
        _boxMin[(quint32)item->raw()].clear();
        for(int band = 0; band < sampleset().sampleRasterSet()->size().zsize(); ++band){
            double std = sampleset().statistics()->at(item->raw(),band, SampleCell::mSTANDARDDEV);
            double mean = sampleset().statistics()->at(item->raw(),band, SampleCell::mMEAN);

            _boxMax[(quint32)item->raw()].push_back(mean + _widenFactor * std);
            _boxMin[(quint32)item->raw()].push_back(mean - _widenFactor * std);
        }
    }
    return true;
}

MinDistClassifier::MinDistClassifier(double threshold, const SampleSet& sampleset) : Classifier(sampleset), _threshold(threshold) {

}

bool MinDistClassifier::classify(PixelIterator& iterIn, PixelIterator& iterOut) const
{
    double threshold2;
    if (_threshold != rUNDEF)
        threshold2 = _threshold * _threshold;
    else
        threshold2 = DBL_MAX;

    bool rounding = false;
    Ilwis::IDomain dom = sampleset().sampleRasterSet()->datadefRef().domain<>();
    if (dom.isValid() && dom->name() == "image")
        rounding = true;

    PixelIterator iterEnd = iterOut.end();
    std::vector<double> zcolumn(sampleset().sampleRasterSet()->size().zsize(), rUNDEF);
    while (iterOut != iterEnd) {
        for (double& v : zcolumn) {
            v = *iterIn;
            ++iterIn;
        }
        double rMinDist = DBL_MAX;
        Raw raw = rUNDEF;
        for (auto item : sampleset().thematicDomain()) {
            if (sampleset().smplSum().pixelInClass(item->raw()) <= 0)
                continue;
            double rDist = 0;
            double d;
            for (int band = 0; band < sampleset().sampleRasterSet()->size().zsize(); ++band) {
                d = zcolumn[band] - (rounding ? (qint64)floor(0.5 + sampleset().statistics()->at(item->raw(), band, SampleCell::mMEAN)) : sampleset().statistics()->at(item->raw(), band, SampleCell::mMEAN));
                d *= d;
                rDist += d;
            }
            if ((rDist < rMinDist) && (rDist < threshold2)) {
                raw = item->raw();
                rMinDist = rDist;
            }
        }
        *iterOut = raw;
        ++iterOut;
    }

    return true;
}

bool MinDistClassifier::prepare()
{
    if ((_threshold <= 0) && (_threshold != rUNDEF)) {
        return ERROR2(ERR_ILLEGAL_VALUE_2, "Threshold", QString(TR("%1; Positive threshold needed")).arg(_threshold));
    }

    return true;
}
