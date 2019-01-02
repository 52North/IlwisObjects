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

#ifndef ScharrDERIVATIVES_H
#define ScharrDERIVATIVES_H

namespace Ilwis {
namespace OpenCV {


class ScharrFilter : public OpenCVOperation
{
public:
    ScharrFilter();


    ScharrFilter(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(ScharrFilter);

private:
    quint8 _xorder;
    quint8 _yorder;
    qint8 _sourcedepth;
    qint8 _destdepth;

    bool doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const;
};
}
}

#endif // ScharrDERIVATIVES_H
