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

#ifndef OPENCVHELPER_H
#define OPENCVHELPER_H

namespace Ilwis {
namespace OpenCV {
class OpenCVHelper
{
public:
    OpenCVHelper();
    static bool raster2Mat(Ilwis::PixelIterator rasterIter, cv::Mat &cvRaster);
    static Ilwis::IRasterCoverage mat2Raster(const cv::Mat &cvRaster, const Ilwis::IGeoReference &grf);
    static quint32 ilwisType2OpenCVType(IlwisTypes tp);
    static IlwisTypes openCVType2IlwisType(quint32 cvtype);
    static bool mat2Raster(const cv::Mat &cvRaster, Ilwis::PixelIterator &iter);

    template<typename T> static void copy2Raster(const cv::Mat& cvRaster,Ilwis::PixelIterator iter, double& mmin, double& mmax) {

        for(int i = 0; i < cvRaster.rows; i++)
        {
            const T* mi = cvRaster.ptr<T>(i);
            for(int j = 0; j < cvRaster.cols; j++){
                T v = mi[j];
               *iter = v;
                mmin = Ilwis::min((double)v, mmin);
                mmax = Ilwis::max((double)v, mmax);
                ++iter;
            }

        }
    }
    static bool determineCVType(IlwisTypes valuetype, qint8 &sourcedepth);
    static void createHistogram(Ilwis::PixelIterator rasterIter, cv::SparseMat &histogram, bool accumulate);
};
}
}

#endif // OPENCVHELPER_H
