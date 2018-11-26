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

#ifndef RANGE_H
#define RANGE_H

#include <QSharedPointer>
#include "kernel_global.h"

namespace Ilwis {

typedef std::map<quint64, quint64> RenumberMap;
/*!
 * \brief The Range class base interface for all objects that need to define a range of values.
 *
 *Examples of dervatives of this class are numerical ranges, thematical collections, time intervals. The class itself has not that usefull an interface but pointers to this class can be used as a
 *a general container for more specialized ranges .
 */

class KERNELSHARED_EXPORT Range
{
public:
    Range();
    virtual ~Range() {}

    /*!
     determines the validity of a range. This method must be implemented by each deravative
     * \return true for a valid range
     */
    virtual bool isValid() const = 0;
    /*!
    translates the string into a string representation. As each type of range has a different string representation this method has to be reimplemented by each derivative
     * \return a string
     */
    virtual QString toString() const = 0;
    /*!
     * \brief valueType returns the type of values contained in the range
     * \return an ilwistype
     */
    virtual IlwisTypes valueType() const = 0;

     /*!
     copies the range to a new instance. This pointer has to be deleted by the caller
     * \return the new Range
     */
    virtual Range *clone() const = 0;
    template<typename T> T *merge(const QSharedPointer<T>& range1,const QSharedPointer<T>& range2,RenumberMap *rnm=0){
        T::merge(range1, range2, rnm);
    }

    virtual QVariant ensure(const QVariant& v, bool inclusive = true) const { return rUNDEF; }

    virtual bool contains(const QVariant& v, bool inclusive = true) const = 0;

    virtual bool isContinuous() const;
    void interpolation(const QString& ip);
    virtual QString interpolation() const;
    virtual QVariant impliedValue(const QVariant& v) const { return v; }
    virtual void add(const QVariant& v) = 0;
    virtual quint32 count() const = 0;
    virtual void store(QDataStream& stream) = 0;
    virtual void load(QDataStream& stream) = 0;
    virtual void clear() = 0;

    template<typename T> T* as(){
        return (dynamic_cast<T *>(this));
    }

    template<typename T> const T* as() const{
         return (dynamic_cast<const T *>(this));
    }


    static Range *create(IlwisTypes types);
protected:
    QString _interpolation;



};
typedef QSharedPointer<Range> SPRange;
}

#endif // RANGE_H
