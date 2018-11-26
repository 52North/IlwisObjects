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

#ifndef GRIDINTERPOLATOR_H
#define GRIDINTERPOLATOR_H

namespace Ilwis {
class Grid;

class KERNELSHARED_EXPORT RasterInterpolator
{
public:
    enum InterpolationMethod{ipNEARESTNEIGHBOUR, ipBILINEAR, ipBICUBIC};

    RasterInterpolator(const IRasterCoverage& raster, int method) ;
    double pix2value(const Pixeld &pix);
    double coord2value(const Coordinate& crd, quint32 z);

private:
    double bilinear(const Pixeld &pix) ;
    double bicubic(const Pixeld &pix) ;
    double bicubicPolynom(double values[], const double &delta);
    double bicubicResult(long row, long column, long z, const double &deltaCol);
    bool resolveRealUndefs(double values[]);
    long _nbrows[4], _nbcols[4];
    double _weight[4];
    double _yvalues[4], _xvalues[4];
    IRasterCoverage _gcoverage;
    const UPGrid &_grid; // for peformance reason we store this; will be valid aslong as the coverage is there
    IGeoReference _grf;
    int _method;
    bool _valid;
};
}

#endif // GRIDINTERPOLATOR_H
