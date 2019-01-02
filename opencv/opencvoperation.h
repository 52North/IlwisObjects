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

#ifndef OPENCVOPERATION_H
#define OPENCVOPERATION_H

namespace Ilwis {
namespace OpenCV {


class OpenCVOperation : public OperationImplementation
{
public:
    OpenCVOperation();
    OpenCVOperation(quint64 metaid, const Ilwis::OperationExpression &expr);
    virtual bool doOperation(cv::Mat& inputRaster, cv::Mat& outputRaster) const{ return false; }

    bool execute(ExecutionContext *ctx, SymbolTable &symTable);
protected:
    IRasterCoverage _inputRaster;
    IRasterCoverage _outputRaster;
    bool _resolveUndefs = false;

    void createInputOutputRasters(quint64 copyProperties);
};
}
}

#endif // OPENCVOPERATION_H
