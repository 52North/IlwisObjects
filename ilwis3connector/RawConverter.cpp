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
#include "rawconverter.h"

using namespace Ilwis;
using namespace Ilwis3;

RawConverter::RawConverter(double low, double high, double step, bool hasUndefs)  {
    _storeType =  minNeededStoreType(low, high, step, hasUndefs);
    _offset = determineOffset(low, high, step, _storeType, hasUndefs);
    _scale = MathHelper::roundTo3DecimalDigits ((step == 0.0) ? 1.0 : step);

    // Note: determineScale() is not called here,
    // because _scale gets the value passed-in as step, which in this case is
    // the resolution calculated beforehand

    _undefined = guessUndef(step);
}

IlwisTypes RawConverter::minNeededStoreType(double low, double high, double step, bool hasUndefs) const{
    double minDivStep;
    double maxDivStep;
    intRange(low, high, step, minDivStep, maxDivStep );

    quint64 delta = rounding(abs(maxDivStep - minDivStep));//TODO change from quint32 to quint64 might change behaviour??
    if ( step != 0) {
        if ( delta <= (hasUndefs ? 254 : 255)) // if there is an undef, restrict to one less than 255, to make space for the undef
            return itUINT8;
        else if ( delta <= (hasUndefs ? 65534 : 65535)) // like above: one less ..
            return itINT16;
        else if ( delta <= (hasUndefs ? 4294967294 : 4294967295)){ // like above: one less ..
            return itINT32;
        }
    }
    return itDOUBLE;

}

long RawConverter::rounding(double x) const {
    return (long)floor(x + 0.5);
}

void RawConverter::intRange(double low, double high, double step, double& minDivStep, double& maxDivStep) const {
    minDivStep = low;
    maxDivStep = high;
    if (step > 1e-30) {
        minDivStep /= step;
        maxDivStep /= step;
    }
}

double RawConverter::determineScale(double low, double high, double step) const  {
    if (step == 0.0)
        return 1.0;
    double minDivStep;
    double maxDivStep;
    intRange(low, high, step, minDivStep, maxDivStep );
    //int r = log10(abs(maxDivStep - minDivStep)) + 1;
    // try this....
    int r = log10(rounding(abs(maxDivStep - minDivStep)));
    return pow(10,-r);
}

double RawConverter::determineOffset(double low, double high, double step, IlwisTypes st, bool hasUndefs)
{
  if (st == itDOUBLE)
    return 0;
  double minDivStep;
  double maxDivStep;
  intRange(low, high, step, minDivStep, maxDivStep );

  double r0 = 0;
  double r = rounding(minDivStep / 2 + maxDivStep / 2 - 0.0001);
  r0 = r;
  switch (st) {
     case itINT32:
       if (minDivStep < -LONG_MAX || maxDivStep > LONG_MAX)
         r0 = minDivStep / 2 + maxDivStep / 2 - 0.0001;
       else
         r0 = step != 1 ? -0.001 : 0;
       break;
     case itINT16:
       if (minDivStep < -SHRT_MAX || maxDivStep > SHRT_MAX)
         r0 = minDivStep / 2 + maxDivStep / 2 - 0.0001;
       else
         r0 = step != 1 ? -0.001 : 0;
       break;
     case itUINT8:
       if (minDivStep < 0 || maxDivStep > 255)
         r0 = minDivStep;
       else
         r0 = 0;
       if (hasUndefs)
           r0 -= 1;
       break;
  }
  return r0;
}

double RawConverter::guessUndef(double step) {
    if (step != 0) {
        if (_storeType == itUINT8)
           return 0;
        if ( _storeType == itINT16)
           return shILW3UNDEF;
        else if ( _storeType == itINT32)
            return iILW3UNDEF;
        if ( _storeType == itFLOAT)
            return flUNDEF;
    }
    return rUNDEF;
}

