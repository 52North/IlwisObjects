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
#include "eigen3/Eigen/LU"
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
            for (int band = 0; band < sampleset().sampleRasterSet()->size().zsize(); ++band) {
                double d = zcolumn[band] - (rounding ? (qint64)floor(0.5 + sampleset().statistics()->at(item->raw(), band, SampleCell::mMEAN)) : sampleset().statistics()->at(item->raw(), band, SampleCell::mMEAN));
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

MinMahaDistClassifier::MinMahaDistClassifier(double threshold, const SampleSet& sampleset) : Classifier(sampleset), _threshold(threshold) {

}

double MinMahaDistClassifier::rAdd(quint32 /*iClass*/) const
{
    return 0;
}

bool MinMahaDistClassifier::classify(PixelIterator& iterIn, PixelIterator& iterOut) const
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

    quint32 iBands = sampleset().sampleRasterSet()->size().zsize();

    PixelIterator iterEnd = iterOut.end();
    std::vector<double> zcolumn(iBands, rUNDEF);
    while (iterOut != iterEnd) {
        for (double& v : zcolumn) {
            v = *iterIn;
            ++iterIn;
        }
        double rMinDist = DBL_MAX;
        Raw raw = rUNDEF;
        for (auto item : sampleset().thematicDomain()) {
            quint32 cl = item->raw();
            if (sampleset().smplSum().pixelInClass(cl) <= 0)
                continue;
            double rDist = 0;
            // calc x-transposed * varcovinv * x :
            for (int b = 0; b < iBands; ++b) {
                double d = zcolumn[b] - (rounding ? (qint64)floor(0.5 + sampleset().statistics()->at(cl, b, SampleCell::mMEAN)) : sampleset().statistics()->at(cl, b, SampleCell::mMEAN));
                for (int b1 = 0; b1 < iBands; b1++) {
                    double d1 = zcolumn[b1] - (rounding ? (qint64)floor(0.5 + sampleset().statistics()->at(cl, b1, SampleCell::mMEAN)) : sampleset().statistics()->at(cl, b1, SampleCell::mMEAN));
                    rDist += d * d1 * varcovinv.at(cl)(b, b1);
                }
            }
            double rDistX = rDist + rAdd(cl);
            if ((rDistX < rMinDist) && (rDistX < threshold2)) {
                raw = cl;
                rMinDist = rDistX;
            }
        }
        *iterOut = raw;
        ++iterOut;
    }

    return true;
}

bool MinMahaDistClassifier::prepare()
{
    if ((_threshold <= 0) && (_threshold != rUNDEF)) {
        return ERROR2(ERR_ILLEGAL_VALUE_2, "Threshold", QString(TR("%1; Positive threshold needed")).arg(_threshold));
    }

    varcovinv.clear();
    quint32 iBands = sampleset().sampleRasterSet()->size().zsize();
    for (auto item : sampleset().thematicDomain()) {
        quint32 cl = item->raw();
        if (sampleset().smplSum().pixelInClass(cl) <= 0)
            continue;

        varcovinv[cl] = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>(iBands, iBands);
        for (int i = 0; i < iBands; ++i) {
            for (int j = 0; j < iBands; ++j) {
                if (j >= i) {
                    double rTot = sampleset().smplSum().pixelInClass(cl);
                    if (rTot != 0)
                        varcovinv[cl](i, j) = sampleset().smplSumXY().at(cl, i, j) / rTot - (sampleset().smplSum().at(cl, i) / rTot) * (sampleset().smplSum().at(cl, j) / rTot);
                    else
                        varcovinv[cl](i, j) = 0;
                }
                else
                    varcovinv[cl](i, j) = varcovinv[cl](j, i);
            }
        }
        if (abs(varcovinv[cl].determinant()) < EPS10) { // Singularity test; determinant equal (or close) to 0.
            Ilwis::DataDefinition datadef = sampleset().sampleRasterSet()->datadefRef();
            QVariant cellValue = QVariant(cl);
            cellValue = datadef.range<>() ? datadef.range<>()->impliedValue(cellValue) : datadef.domain<>()->impliedValue(cellValue);
            return ERROR2(ERR_ILLEGAL_VALUE_2, "Matrix", QString(TR("Singular Covariance Matrix found. Definite positive variances needed. Class %1 needs more samples. Or bands are linearly dependent.")).arg(cellValue.toString()));
        }
        else
            varcovinv[cl] = varcovinv[cl].inverse(); // eval() ?
    }

    return true;
}

MaxLikelihoodClassifier::MaxLikelihoodClassifier(double threshold, const SampleSet& sampleset) : MinMahaDistClassifier(threshold, sampleset) {

}

double MaxLikelihoodClassifier::rAdd(quint32 iClass) const
{
    return lnDet.at(iClass);
}

bool MaxLikelihoodClassifier::prepare() {
    if (MinMahaDistClassifier::prepare()) {
        for (auto item : sampleset().thematicDomain()) {
            quint32 cl = item->raw();
            double vcvInvDet = abs(varcovinv[cl].determinant());
            // because the determinant of the inverse matrix equals
            // the inverse of the determinant of the original matrix:
            lnDet[cl] = log(1 / vcvInvDet);
        }
        return true;
    }
    else
        return false;

}

#define ONE_TOL	 1.00000000000001

SpectralAngleClassifier::SpectralAngleClassifier(double threshold, const SampleSet& sampleset) : Classifier(sampleset), _threshold(threshold) {

}

double SpectralAngleClassifier::aacos(double v) const
{
    double av;
    if ((av = abs(v)) >= 1.) {
        if (av > ONE_TOL)
            return rUNDEF;
        return (v < 0. ? M_PI : 0.);
    }
    return acos(v);
}

bool SpectralAngleClassifier::classify(PixelIterator& iterIn, PixelIterator& iterOut) const
{
    PixelIterator iterEnd = iterOut.end();
    std::vector<double> zcolumn(sampleset().sampleRasterSet()->size().zsize(), rUNDEF);
    while (iterOut != iterEnd) {
        for (double& v : zcolumn) {
            v = *iterIn;
            ++iterIn;
        }
        double rMinAngle = DBL_MAX;
        Raw raw = rUNDEF;
        for (auto item : sampleset().thematicDomain()) {
            if (sampleset().smplSum().pixelInClass(item->raw()) <= 0)
                continue;
            double rSumT2 = 0; // norm terms for vector T
            double rSumM2 = 0; // norm terms for M
            double rSumTM = 0; // inproduct terms
            for (int band = 0; band < sampleset().sampleRasterSet()->size().zsize(); ++band) {
                double rT = zcolumn[band];
                double rM = sampleset().statistics()->at(item->raw(), band, SampleCell::mMEAN);
                rSumT2 += rT * rT;
                rSumM2 += rM * rM;
                rSumTM += rT * rM;
            }
            double rAngle = aacos(rSumTM / sqrt(rSumT2 * rSumM2)); //angle between vectors T and class mean M in iBands()-dimensional space
            if ((rAngle < rMinAngle) && (rAngle < _threshold)) {
                raw = item->raw();
                rMinAngle = rAngle;
            }
        }
        *iterOut = raw;
        ++iterOut;
    }

    return true;
}

bool SpectralAngleClassifier::prepare()
{
    if ((_threshold <= 0) && (_threshold != rUNDEF)) {
        return ERROR2(ERR_ILLEGAL_VALUE_2, "Threshold", QString(TR("%1; Positive threshold needed")).arg(_threshold));
    }

    return true;
}
