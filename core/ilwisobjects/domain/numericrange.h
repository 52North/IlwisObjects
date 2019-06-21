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

#ifndef NUMERICVALUERANGE_H
#define NUMERICVALUERANGE_H

#include <QString>
#include "range.h"
#include "rangeiterator.h"

namespace Ilwis {

/*!
 * \brief The NumericRange class describes a range of numbers between a minimum and maximum combined with a resolution
 */
class KERNELSHARED_EXPORT NumericRange : public Range
{
public:
    NumericRange();
    /*!
     * \brief NumericRange basic constructor for the range with a minimum and a maximum and a resolution.
     *
     *If the resolution is 0, the range has same resolution a double. A step size of 0 will make the range iterator invalid. This prefectly legal, it only makes the iterator not usable.
     *A resolution of an integer will make the range an integer range
     *
     * \param mi minimum of the range
     * \param ma maximum of the range
     * \param step resolution of the range.
     */
    NumericRange(double mi, double ma, double resolution=0);
    /*!
     * \brief NumericRange copy constructor
     * \param vr other value range
     */
    NumericRange(const NumericRange &vr);
    NumericRange(const QString& def);


    bool isValid() const;
    Range *clone() const ;

    bool contains(double v, bool inclusive = true) const{
        if (!isValid())
            return false;
        if ( isNumericalUndef(v))
            return false;

        bool ok = inclusive ? v >= _min && v <= _max : v > _min && v < _max;
        if (!ok || _resolution < 0.01 || _resolution == 1)
            return ok;
        double floattest = (v - _min) / _resolution;
        double inttest = round(floattest);
        double delta = fabs(floattest - inttest);
        return delta < EPS7; // EPS10 is too much to ask; the computed values aren't that accurate
    }


    bool contains(const QVariant &value, bool inclusive = true) const;
    bool contains(SPRange rng, bool inclusive=true) const;
    bool contains(NumericRange *rng, bool inclusive=true) const;
	bool overlaps(const NumericRange *nrange, bool strict = true) const{
		if (!nrange)
			return false;

		if (strict) {
			return nrange->min() >= min() && nrange->min() <= max() || // if the external min is between the min and max it will always overlap
				nrange->max() >= min() && nrange->max() <= max(); // same for the max
		} 
		double d1 = nrange->min() - min();
		double d2 = nrange->min() - max();
		double d3 = nrange->max() - min();
		double d4 = nrange->max() - max();
		bool ret = d1 >0 && d2 <0 ||
			d3 > 0 && d4 < 0;
		return ret; 
	}
    double max() const;
    void max(double v);
    double min() const;
    void min(double v);
    double distance() const;
    double center() const;
    void add(const QVariant& number);

    void resolution(double resolution);
    double resolution() const ;

    NumericRange& operator+=(double v);
    void add(double v);

    bool operator==(const NumericRange& vr);
    bool operator<(const NumericRange& vr);
    bool operator>(const NumericRange& vr);
    bool operator >=(const NumericRange &vr);
    bool operator <=(const NumericRange &vr);

    QString toString() const ;
    QVariant impliedValue(const QVariant& v) const;
    IlwisTypes valueType() const;
    IlwisTypes determineType(bool redo=false) ;
    void set(const NumericRange& vr);
    QVariant ensure(const QVariant& v, bool inclusive=true) const
    {
        double value = v.toDouble();
        if ( _resolution != 0.0)
             value = (double)((qint64)(value / _resolution)) * _resolution;
        if ( !contains(value, inclusive))
            return _undefined;

        return value;
    }

    void clear();
    quint32 count() const;
    void store(QDataStream& stream);
    void load(QDataStream& stream);

    static NumericRange *merge(const QSharedPointer<NumericRange>& nr1, const QSharedPointer<NumericRange>& nr2,RenumberMap *rnm=0);

    static double valueAt(quint32& index, const Range *rng);

    static QString valueAsString(quint32 &index, const Range *rng);
private:

    double _min;
    double _max;
    double _resolution;
    double _undefined;
    IlwisTypes _valuetype = itUNKNOWN;

    long significantDigits(double m1) const;

};

typedef QSharedPointer<NumericRange> SPNumericRange;
typedef Ilwis::RangeIterator<double, Ilwis::NumericRange> NumericRangeIterator;

inline NumericRangeIterator begin(const Ilwis::NumericRange& rng) {
    return NumericRangeIterator(&rng);
}

inline NumericRangeIterator end(const Ilwis::NumericRange& rng) {
    auto iter = NumericRangeIterator(&rng);
    iter += rng.count();
    return iter;
}


}

Q_DECLARE_METATYPE(Ilwis::NumericRange)
Q_DECLARE_METATYPE(Ilwis::SPNumericRange)





#endif // NUMERICVALUERANGE_H
