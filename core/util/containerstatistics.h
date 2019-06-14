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

#ifndef ContainerStatistics_H
#define ContainerStatistics_H

#include "kernel.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/sum.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/kurtosis.hpp>
#include "tranquilizer.h"

#include "mathhelper.h"

using namespace boost::accumulators;

namespace Ilwis{

template<typename DataType> class ContainerStatistics
{
public:
    typedef accumulator_set<DataType, stats<tag::mean,tag::min, tag::max, tag::count>> Basic;
    typedef accumulator_set<DataType, stats<tag::median>> Median;
    typedef accumulator_set<double, stats<tag::mean, tag::moment<2> > > Variance;
    typedef accumulator_set<double, stats<tag::mean, tag::moment<3> > > Skew;
    typedef accumulator_set<int, stats<tag::kurtosis > > Kurtosis;

    struct HistogramBin{
        HistogramBin(DataType limit=0, quint64 count=0) : _limit(limit), _count(count) {}

        DataType _limit;
        quint64 _count;
    };

    enum PropertySets{pNONE = 0, pBASIC=1, pMIN=2, pMAX=4, pDISTANCE=8, pDELTA=16,pNETTOCOUNT=32, pCOUNT=64, pSUM=128,
                      pMEAN=256, pMEDIAN=512, pPREDOMINANT=1024, pSTDEV=2048, pHISTOGRAM=4096, pVARIANCE=8192, pSKEW=16384,pKURTOSIS=32768,
					  pQUICKHISTOGRAM =65536,pLAST= 131072, pALL=2147483647};
    static const quint32 pNUMERICS = pMIN | pMAX | pDISTANCE | pDELTA | pNETTOCOUNT | pCOUNT | pSUM | pMEAN | pMEDIAN | pPREDOMINANT | pVARIANCE | pSKEW | pKURTOSIS;

    ContainerStatistics(){
        _sigDigits = shUNDEF;
        _markers.resize(index(pLAST));
        DataType undefined = undef<DataType>();
        std::fill(_markers.begin(), _markers.end(), undefined);
    }

    inline double operator[](PropertySets method) const{
        if ( method == 0)
            return rUNDEF;

        quint32 ind = index(method);
        if ( ind < _markers.size()) {
            if ( hasType(method, pMEAN | pMIN | pMAX | pMEDIAN))
                return (DataType)_markers[ind];

            return _markers[ind]    ;
        }
        return rUNDEF;
    }

    double operator()(PropertySets method, double limit){
        if (_bins.size() == 0)    {
            return rUNDEF;
        }

        limit = _markers[index(pNETTOCOUNT)] * (limit / 100.0);
        double sum = 0;
        if ( method == pMIN){
            for(int i = 0 ; i < _bins.size(); ++i){
                sum += _bins[i]._count;
                if ( sum > limit){
                    return ((double)i / _bins.size()) * ( _markers[index(pMAX)] - _markers[index(pMIN)]) + _markers[index(pMIN)];
                }
            }
        } else if ( method == pMAX){
            double totalSum = _markers[index(pNETTOCOUNT)];
            sum = _markers[index(pNETTOCOUNT)];
            for(int i = _bins.size() - 2 ; i > 0; --i){
                sum -= _bins[i]._count;
                if ( sum < totalSum - limit){
                    return ((double)i / _bins.size()) * ( _markers[index(pMAX)] - _markers[index(pMIN)]) + _markers[index(pMIN)];
                }
            }
        }
        return rUNDEF;

    }

    std::vector<HistogramBin> histogram() const {
        return _bins;
    }

	void setContent(const std::vector<HistogramBin>& bins, const std::vector<double> markers) {
		_bins = bins;
		_markers = markers;
		_binCount = bins.size();

	}

	std::vector<double> markers() const {
		return _markers;
	}


    double prop(PropertySets method) const{
        return operator[](method);
    }

    quint16 significantDigits() const {
        return _sigDigits;
    }


    template<typename IterType> quint32 findSignificantDigits(const IterType& begin,  const IterType& end, DataType undefined) {
        quint32 numSigDecimalDigits = 0;

        std::for_each(begin, end, [&] (const DataType& sample){
            if ( sample != undefined) {
                quint32 lenDecimalDigits = MathHelper::lenDecimalDigits(sample );
                numSigDecimalDigits = std::max(numSigDecimalDigits, lenDecimalDigits);
            }
        });
        return numSigDecimalDigits;
    }

    void binCount(quint32 value) {
        _binCount = value;
    }



    template<typename IterType> bool calculate(const IterType& begin, const IterType& end, std::unique_ptr<Tranquilizer>& tranquilizer, int mode = pBASIC, int bins = 0, double pseudoUndef = rILLEGAL) {
        //Basic basicMarkers;
        Median median;
        Variance var;
        Skew skew;
        Kurtosis kurt;
        DataType undefined;
        undefined = pseudoUndef != rILLEGAL ? undef<DataType>() : pseudoUndef;
        double sigDigits = 0;
        double rest = 0;

        double pmin = rUNDEF, pmax = rUNDEF, psum = 0;
        quint64 pcount=0, pnetcount = 0;
        kernel()->startClock();
        for (auto iter = begin; iter != end; ++iter) {
            DataType sample = *iter;
			++pcount;
            if (!isNumericalUndef(sample)) {
                rest = fabs(sample - (qint64)sample);
                sigDigits = std::max(sigDigits, rest - sigDigits);
                pmin = Ilwis::min(pmin, sample);
                pmax = Ilwis::max(pmax, sample);
                ++pnetcount;
                psum += sample;

                if (hasType(mode, pVARIANCE))
                    var(sample);
                if (hasType(mode, pMEDIAN)) {
                    median(sample);
                }
                if (hasType(mode, pSKEW)) {
                    skew(sample);
                }
                if (hasType(mode, pKURTOSIS)) {
                    kurt(sample);
                }
                if (tranquilizer.get() != 0)
                    tranquilizer->update(1);

            }
        }
		kernel()->endClock();
		kernel()->startClock();
        bool isUndefined = pnetcount == 0;
        std::fill(_markers.begin(), _markers.end(), rUNDEF);
		_markers[index(pCOUNT)] = pcount;
		_markers[index(pNETTOCOUNT)] = pnetcount;
        if (!isUndefined) {
            _markers[index(pMIN)] = pmin;
            _markers[index(pMAX)] = pmax;
            _markers[index(pDISTANCE)] = std::abs(prop(pMAX) - prop(pMIN));
            _markers[index(pDELTA)] = prop(pMAX) - prop(pMIN);
            _markers[index(pSUM)] = psum;
            _markers[index(pMEAN)] = pnetcount > 0 ? psum / pnetcount : rUNDEF;
            _markers[index(pMEDIAN)] = boost::accumulators::median(median);
            _markers[index(pVARIANCE)] = boost::accumulators::moment<2>(var);
            _markers[index(pSKEW)] = boost::accumulators::moment<3>(skew);
            _markers[index(pKURTOSIS)] = boost::accumulators::kurtosis(kurt);
            //_sigDigits = findSignificantDigits(begin, end, undefined);

            if (mode & pSTDEV) {
                _markers[index(pSTDEV)] = calcStdDev(begin, end, undefined);
            }
            if (hasType(mode, pHISTOGRAM| pQUICKHISTOGRAM)) {

                double ncount = prop(pNETTOCOUNT);
                if (ncount > 1) {
                    if (bins == 0 && _binCount == iUNDEF) {
                     /*   if (prop(pSTDEV) == rUNDEF) {
                            _markers[index(pSTDEV)] = calcStdDev(begin, end, undefined);
                        }
                        if (_markers[index(pSTDEV)] != rUNDEF) {
                            double h = 3.5 * _markers[index(pSTDEV)] / pow(ncount, 0.3333);
                            _binCount = prop(pDISTANCE) / h;
                        }*/
						_binCount = 5 * std::sqrt(std::sqrt(pnetcount)) + 1;
                    }
                    else if (bins != 0) {
                        _binCount = bins - 1;
                    }
                }

                _bins.resize(_binCount + 2); // last cell is for undefines
                double delta = prop(pDELTA);
                for (int i = 0; i < _binCount; ++i) {
                    _bins[i] = HistogramBin(prop(pMIN) + i * (delta / _binCount));
                }
                _bins[_binCount] = HistogramBin(prop(pMAX));
                double rmin = prop(pMIN);
                double rdelta = prop(pDELTA);
				int binsize = _bins.size();
                for (auto iter = begin; iter != end; ++iter) {
                    DataType sample = *iter;
                    quint16 index = (quint16)binsize - 1;
                    if (!isNumericalUndef(sample)) {
						double d = (double)(sample - rmin);
						double idx = (double)binsize * d / rdelta;
						index = idx;
                        index =  index >= binsize-2 ? index - 2 : index; // -2 is the last 'real' number, -1 is the place for undefs; through rounding the index may endup at index -1 which is not what we want;
                    }
                    _bins.at(index)._count++;
                }
            }
        }
        kernel()->endClock();

        return true;
    }

        template<typename IterType> bool calculate(const IterType& begin,  const IterType& end, int mode=pBASIC, int bins = 0, double pseudoUndef=rILLEGAL){
            std::unique_ptr<Tranquilizer> tranquilizer;
            return calculate(begin, end, tranquilizer, mode, bins, pseudoUndef);
        }

        bool isValid() const {
            return prop(pMAX) != rUNDEF;
        }

        double stretchLinear(double input, int stretchRange) const {
            if ( input == rUNDEF)
            return rUNDEF;

            double stretchFactor = stretchRange / prop(pDELTA);
            double rmin = prop(pMIN);
            double rdelta = prop(pDELTA);
            quint16 index = getOffsetFactorFor(input, rmin, rdelta);
            return _bins[index]._limit * stretchFactor;
        }

        /**
     * Stretches the value range by percent. Lower and upper limits are stretched equally.
     * If stretch value is undefined, the limits of the current value range are returned.
     *
     * @brief stretches lower and upper limits by percent.
     * @param percent how much the limits shall be stretched.
     * @return stretched limits of the value range.
     */
        std::pair<double,double> stretchLimits(double percent) const {
            if (percent == rUNDEF) {
                return std::pair<double,double>(prop(pMIN),prop(pMAX));
            }
            double delta = prop(pDELTA);
            double downsizeBy = percent * delta / 100;
            double newLower = prop(pMIN) + downsizeBy;
            double newUpper = prop(pMAX) - downsizeBy;
            return std::pair<double,double>(newLower,newUpper);
        }

		std::pair<double, double> calcStretchRange(double perc) const {
			double sum2 = 0;
			double seen = 0;
			double startV = rUNDEF, endV = rUNDEF;

			if (_bins.size() > 0) {
				for (int i = 0; i < _bins.size() - 1; ++i) {
					sum2 += (_bins[i]._count);
				}
				for (int i = 0; i < _bins.size() - 1; ++i) {
					auto& bin = _bins[i];
					seen += bin._count;
					if (seen >= sum2 * perc && startV == rUNDEF) {
						startV = bin._limit;
					}
					if (seen >= sum2 * (1.0 - perc) && endV == rUNDEF) {
						endV = bin._limit;
					}
				}
			}
			return std::pair<double, double>(startV, endV);
		}

        private:
        std::vector<double> _markers;

        quint32 _sigDigits;
        std::vector<HistogramBin> _bins;
        quint32 _binCount=iUNDEF;

        quint32 index(PropertySets method) const {
            if ( method == 0)
                return 0;
            return (quint32)(std::log((int)method) / log(2) + 0.2);
        }

        template<typename IterType> double calcStdDev(const IterType& begin,  const IterType& end, DataType undefined) {
            double ncount = prop(pNETTOCOUNT);
            if ( ncount < 2)
                return rUNDEF;
            double acc = 0;
            double mean = prop(pMEAN);
            std::for_each(begin, end, [&] (const DataType& sample){
                if ( sample != undefined)
                    acc += (sample - mean) * (sample - mean);
            });
            return sqrt(acc / (ncount-1));

        }

        quint16 getOffsetFactorFor(const DataType& sample, double rmin, double rdelta) const {
            quint16 index = _bins.size() * (double)(sample - rmin) / rdelta;
            return index == _bins.size() ? index - 1 : index;
        }

        double getBinWidth() const {
            if (_bins.size() > 1) {
                return _bins[1]._limit;
            } else {
                return _bins[0]._limit;
            }
        }
};
typedef ContainerStatistics<double> NumericStatistics;
}

#endif // ContainerStatistics_H
