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

#ifndef OPERATIONOVERLOADS_H
#define OPERATIONOVERLOADS_H
#include "kernel_global.h"

KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator+(const Ilwis::IRasterCoverage& raster1, double);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator-(const Ilwis::IRasterCoverage& raster1, double);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator*(const Ilwis::IRasterCoverage& raster1, double);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator/(const Ilwis::IRasterCoverage& raster1, double);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator+(const Ilwis::IRasterCoverage& raster1, const Ilwis::IRasterCoverage& raster2);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator*(const Ilwis::IRasterCoverage& raster1, const Ilwis::IRasterCoverage& raster2);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator/(const Ilwis::IRasterCoverage& raster1, const Ilwis::IRasterCoverage& raster2);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator-(const Ilwis::IRasterCoverage& raster1, const Ilwis::IRasterCoverage& raster2);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator/(double number,const Ilwis::IRasterCoverage &raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator-(double number,const Ilwis::IRasterCoverage &raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage operator<(const Ilwis::IRasterCoverage& raster1, double);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage sin(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage cos(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage tan(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage asin(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage acos(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage atan(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage log10(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage sqrt(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage ln(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage ceil(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage floor(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage sign(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage cosh(const Ilwis::IRasterCoverage& raster1);
KERNELSHARED_EXPORT Ilwis::IRasterCoverage sinh(const Ilwis::IRasterCoverage& raster1);


#endif // OPERATIONOVERLOADS_H
