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

#ifndef SOBELDERIVATIVES_H
#define SOBELDERIVATIVES_H

namespace Ilwis {
namespace OpenCV {


class SobelFilter : public OpenCVOperation
{
public:
    SobelFilter();


    SobelFilter(quint64 metaid, const Ilwis::OperationExpression &expr);

    //bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(SobelFilter);

private:
    quint8 _xorder;
    quint8 _yorder;
    quint8 _kernelSize;
    qint8 _sourcedepth;
    qint8 _destdepth;


    bool doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const;
};
}
}

#endif // SOBELDERIVATIVES_H
