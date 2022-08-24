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

#ifndef RAWCONVERTER_H
#define RAWCONVERTER_H

#include "kernel.h"
#include "ilwis.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "georefimplementation.h"
#include "factory.h"
#include "abstractfactory.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "controlpoint.h"
#include "ctpgeoreference.h"
#include "mathhelper.h"

namespace Ilwis {
namespace Ilwis3{

const short  shILW3UNDEF = std::numeric_limits < qint16 >::min() + 1;
const long   iILW3UNDEF  = std::numeric_limits < qint32 >::min() + 1;

class RawConverter
{
public:
    RawConverter(const QString &type)
    {
        _scale = 1;
        _offset = -1;
        _undefined = 0;
        if(type == "color"){
            _storeType = itUINT32;
            _colors = true;
        }
        if ( type == "class" || type == "group") {
            _storeType = itUINT8;
            _item = true;
        } else if ( type == "ident"){
            _storeType = itUINT16;
            _item = true;
        } else
            _storeType = itUINT32;
    }



    RawConverter() : _offset(0), _scale(1), _storeType(itUNKNOWN), _undefined(rUNDEF){
    }

    RawConverter(double offset, double scale, double low, double high, IlwisTypes st=itUNKNOWN) : _offset(offset), _scale(scale), _storeType(st){
        if ( st == itUNKNOWN)
            _undefined = guessUndef(scale);
        else {
            switch(_storeType) {
                case itINT16:
                    _undefined = shILW3UNDEF; break;
                case itINT32:
                    _undefined = iILW3UNDEF; break;
                case itDOUBLE:
                    _undefined = -1e308; break;
                default:
                    _undefined = 0;
            }
        }
    }

    RawConverter(double low, double high, double step, bool hasUndefs);


    double raw2real(double raw) const{
        if (_colors){
            if ( raw == iILW3UNDEF)
                return clrUNDEF2;
            return ( (quint32)raw | 0xFF000000); // setting transparency bit to 255 as this is by default not present

        }        
        if (( _item && raw == 0) || (raw == iILW3UNDEF || raw == shILW3UNDEF))
            return rUNDEF;
        if (raw == _undefined)
            return rUNDEF;
        return MathHelper::roundTo3DecimalDigits ((raw + _offset) * _scale);
    }
    double real2raw(double real) const {
        if ( real == rUNDEF)
            return _undefined;
        if (_scale == 0)
            return real;
        return MathHelper::roundTo3DecimalDigits (real / _scale)   - _offset;
    }
    bool isNeutral() const{
        if ( _storeType == itDOUBLE )
            return true;
        return !_colors && ((_offset == 0 && _scale == 1.0) || _scale == 0);
    }

    double offset() const {
        return _offset;
    }

    double scale() const{
        return _scale;
    }

    IlwisTypes storeType() const{
        return _storeType;
    }

    void  storeType(IlwisTypes st) {
        _storeType = st;
        switch(_storeType) {
            case itINT16:
                _undefined = shILW3UNDEF; break;
            case itINT32:
                _undefined = iILW3UNDEF; break;
            case itDOUBLE:
                _undefined = -1e308; break;
            default:
                _undefined = 0;
        }
    }

    bool isValid() const{
        return _storeType != itUNKNOWN;
    }

private:
    double guessUndef(double step);
    long rounding(double x) const;
    double determineOffset(double low, double high, double step, IlwisTypes st, bool hasUndefs);
    void intRange(double low, double high, double step, double &minDivStep, double &maxDivStep) const;
    double determineScale(double low, double high, double step) const;
    IlwisTypes minNeededStoreType(double low, double high, double step, bool hasUndefs) const;

    qint32 _offset;
    double _scale;
    IlwisTypes _storeType;
    double _undefined;
    bool _colors=false;
    bool _item=false;


};
}
}

#endif // RAWCONVERTER_H
