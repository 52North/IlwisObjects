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

#ifndef GRIDCOVERAGEGENERATOR_H
#define GRIDCOVERAGEGENERATOR_H

namespace Ilwis {

class GridCoverageGenerator;

typedef double (*DFunctionD)(double v);

struct Slice {
    Slice() : _status(false), _minLine(0), _maxline(0), _outputgc(0) {}
    DFunctionD _calc;
    unsigned long _minLine;
    unsigned long _maxline;
    IGridCoverage _inputgc;
    GridCoverageGenerator *_outputgc;
    bool _status;

    void calculateSlice();

};


class GridCoverageGenerator : public GridCoverage
{
public:
    GridCoverageGenerator();
    bool generateFrom(const IGridCoverage &inputGC, DFunctionD func);
    bool setSize(const Box2D<qint32> &box, const Box2D<double> bounds);
    bool setDomain(const QString& name);
};

typedef IlwisData<GridCoverageGenerator> IGridCoverageGenerator;
}

#endif // GRIDCOVERAGEGENERATOR_H
