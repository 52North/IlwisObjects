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
#include <unordered_map>
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

    // Note that there is a copy of the PropertySets enum values in pythonapi_util.h; when updating here, make a copy there
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

    double operator()(PropertySets method, double perc){
        if (_bins.size() == 0)    {
            return rUNDEF;
        }

        if (perc == 0) { // find the first non-zero _count
            if (method == pMIN) {
                for (long i = 0; i < _bins.size() - 1; ++i) { // < size() - 1, as the last element is the undef count.
                    if (_bins[i]._count > 0) {
                        return _bins[i]._limit;
                    }
                }
                return _bins[0]._limit;
            } else if (method == pMAX) {
                for (long i = _bins.size() - 2; i >= 0; --i) { // same here: start at size() - 2 (one before the undef count)
                    if (_bins[i]._count > 0) {
                        return _bins[i]._limit;
                    }
                }
                return _bins[_bins.size() - 2]._limit;
            }
        }

        if ( method == pMIN){
            long nPixCumulative = 0;
            long pixels = perc * _markers[index(pNETTOCOUNT)] / 100.0;
            int lo = 0;
            for(int i = 0 ; i < _bins.size() - 1; ++i){
                nPixCumulative += _bins[i]._count;
                if (nPixCumulative > pixels){
                    lo = i;
                    break;
                }
            }
            if (lo > 0) {
                long prevDif = nPixCumulative - _bins[lo]._count - pixels;
                if (prevDif != 0) // why must prevDif != 0?
                    if (abs(prevDif) < abs(nPixCumulative - pixels))
                        --lo;
            }
            return _bins[lo]._limit;
        } else if ( method == pMAX){
            long nPixCumulative = _markers[index(pNETTOCOUNT)];
            long pixels = (100.0 - perc) * _markers[index(pNETTOCOUNT)] / 100.0;
            int hi = _bins.size() - 2;
            for (int i = _bins.size() - 2; i >= 0; --i) {
                if (nPixCumulative <= pixels) {
                    hi = i;
                    break;
                }
                nPixCumulative -= _bins[i]._count;
            }
            if (hi < _bins.size() - 2) {
                long prevDif = nPixCumulative + _bins[hi + 1]._count - pixels;
                if (prevDif != 0) // same: why must prevDif != 0?
                    if (abs(prevDif) < abs(nPixCumulative - pixels))
                        ++hi;
            }
            return _bins[hi]._limit;
        }
        return rUNDEF;

    }

    std::vector<HistogramBin> histogram() const {
        return _bins;
    }

	void setContent(const std::vector<HistogramBin>& bins, const std::vector<double> markers, PropertySets hmode) {
		_bins = bins;
		_markers = markers;
		_histogramMode = hmode;

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

	int binCount() const {
        if (_bins.size() > 0)
            return _bins.size() - 1; // last is undef
        else
            return 0;
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
		qint32 imax = -1e9, imin = 1e9;
		std::vector<double> vmap; // this map contain a mapping of the rawvalue of the raster to a value from the attribute table; only relevant for rasters with attributes. In other cases this is empty
		if (_mapping.size() > 0) {
			for (auto& item : _mapping) {
				imax = std::max(item.first, imax);
				imin = std::min(item.first, imin);
			}
			vmap.resize(imax+1);
			for (auto& item : _mapping) {
				vmap[item.first] = item.second;
			}
		}

        double pmin = rUNDEF, pmax = rUNDEF, psum = 0;
        quint64 pcount=0, pnetcount = 0;
        for (auto iter = begin; iter != end; ++iter) {
            DataType sample = *iter;
			if (vmap.size() > 0) {
				sample = vmap[(quint32)sample];
			}
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
                    if (bins <= 0) {
                     /*   if (prop(pSTDEV) == rUNDEF) {
                            _markers[index(pSTDEV)] = calcStdDev(begin, end, undefined);
                        }
                        if (_markers[index(pSTDEV)] != rUNDEF) {
                            double h = 3.5 * _markers[index(pSTDEV)] / pow(ncount, 0.3333);
                            _binCount = prop(pDISTANCE) / h;
                        }*/
						bins = 5 * std::sqrt(std::sqrt(pnetcount)) + 1;
						if (std::ceil(psum) == psum) {// integer
							double delta = prop(pDELTA);
							if (delta <= 255) // special case for images
								bins = delta + 1;
							else {
                                double f = delta / (bins - 1);
								double cf = std::ceil(f);
								if (pcount < psum && cf != f && cf > 1.0) {
									bins = 1 + delta / (int)(cf);
								}
							}
						}
                    }
                }

                _bins.resize(bins + 1); // last cell is for undefineds
                double delta = prop(pDELTA);
                for (int i = 0; i < bins - 1; ++i) {
                    _bins[i] = HistogramBin(prop(pMIN) + i * (delta / (bins - 1)));
                }
                _bins[bins - 1] = HistogramBin(prop(pMAX)); // Compute separately, for extra precision
                _bins[bins] = HistogramBin(rUNDEF);
                double rmin = prop(pMIN);
                double rdelta = prop(pDELTA);
				int binsize = _bins.size();
                for (auto iter = begin; iter != end; ++iter) {
                    DataType sample = *iter;
					if (vmap.size() > 0) {
						sample = vmap[(quint32)sample];
					}
                    quint16 index = (quint16)binsize - 1;
                    if (!isNumericalUndef(sample)) {
						double d = (double)(sample - rmin);
						double idx = (double)(binsize-2) * d / rdelta;
						index = idx; // floor
                        index =  index > binsize-2 ? index - 2 : index; // -2 is the last 'real' number, -1 is the place for undefs; through rounding the index may endup at index -1 which is not what we want;
                    }
                    _bins.at(index)._count++;
                }
            }
        }

        return true;
    }

        template<typename IterType> bool calculate(const IterType& begin,  const IterType& end, int mode=pBASIC, int bins = 0, double pseudoUndef=rILLEGAL){
            std::unique_ptr<Tranquilizer> tranquilizer;
            return calculate(begin, end, tranquilizer, mode, bins, pseudoUndef);
        }

		template<typename IterType> bool calculate(const IterType& begin, const IterType& end, std::unordered_map<qint32, double>& mapping, int mode = pBASIC, int bins = 0, double pseudoUndef = rILLEGAL) {
			std::unique_ptr<Tranquilizer> tranquilizer;
			_mapping = mapping;
			return calculate(begin, end, tranquilizer, mode, bins, pseudoUndef);
		}

        bool isValid() const {
            return prop(pMAX) != rUNDEF;
        }

		PropertySets histogramMode() const {
			return _histogramMode;
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
			double startV = rUNDEF, endV = rUNDEF;

            if (_bins.size() > 0) {
                if (perc == 0) {
                    for (long i = 0; i < _bins.size() - 1; ++i) { // < size() - 1, as the last element is the undef count.
                        auto& bin = _bins[i];
                        if (bin._count > 0) {
                            startV = bin._limit;
                            break;
                        }
                    }
                    for (long i = _bins.size() - 2; i >= 0; --i) { // same here: start at size() - 2 (one before the undef count)
                        auto& bin = _bins[i];
                        if (bin._count > 0) {
                            endV = bin._limit;
                            break;
                        }
                    }
                }
                else {
                    double totalPix = 0;
                    for (long i = 0; i < _bins.size() - 1; ++i) {
                        totalPix += (_bins[i]._count);
                    }
                    double nPixCumulative = 0;
                    long pixels = perc * totalPix / 100.0;
                    long lo = 0;
                    for (long i = 0; i < _bins.size() - 1; ++i) {
                        auto& bin = _bins[i];
                        nPixCumulative += bin._count;
                        if (nPixCumulative > pixels) {
                            lo = i;
                            break;
                        }
                    }
                    if (lo > 0) {
                        long prevDif = nPixCumulative - _bins[lo]._count - pixels;
                        if (prevDif != 0)
                            if (abs(prevDif) < abs(nPixCumulative - pixels))
                                --lo;
                    }

                    nPixCumulative = totalPix;
                    pixels = (100.0 - perc) * totalPix / 100.0;
                    long hi = _bins.size() - 2;
                    for (long i = _bins.size() - 2; i >= 0; --i) {
                        if (nPixCumulative <= pixels) {
                            hi = i;
                            break;
                        }
                        auto& bin = _bins[i];
                        nPixCumulative -= bin._count;
                    }
                    if (hi < _bins.size() - 2) {
                        long prevDif = nPixCumulative + _bins[hi + 1]._count - pixels;
                        if (prevDif != 0)
                            if (abs(prevDif) < abs(nPixCumulative - pixels))
                                ++hi;
                    }

                    startV = _bins[lo]._limit;
                    endV = _bins[hi]._limit;
                }
            }
			return std::pair<double, double>(startV, endV);
		}

		static quint32 index(PropertySets method)  {
			if (method == 0)
				return 0;
			return (quint32)(std::log((int)method) / log(2) + 0.2);
		}

        private:
        std::vector<double> _markers;
		std::unordered_map<qint32, double> _mapping;

        quint32 _sigDigits;
        std::vector<HistogramBin> _bins;
		PropertySets _histogramMode = pNONE;

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
            quint16 index = (_bins.size() - 2) * (double)(sample - rmin) / rdelta;
            return index >= _bins.size() ? (index - 1) : index;
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
