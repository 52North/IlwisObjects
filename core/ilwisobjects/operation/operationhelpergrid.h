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

#ifndef OPERATIONHELPERRASTER_H
#define OPERATIONHELPERRASTER_H

namespace Ilwis {

typedef  std::function<bool(const BoundingBox&, int threadIdx)> BoxedAsyncFunc;
class Parameter;

class KERNELSHARED_EXPORT OperationHelperRaster
{
public:
    OperationHelperRaster();
    static BoundingBox initialize(const IRasterCoverage &inputRaster, IRasterCoverage &outputRaster, quint64 what);
    static void subdivideTasks(int cores,const IRasterCoverage& raster, std::vector<BoundingBox > &boxes);
    static bool resample(IRasterCoverage& input1, IRasterCoverage& input2, ExecutionContext *ctx);
    static IRasterCoverage resample(const IRasterCoverage& sourceRaster, const IGeoReference& targetGrf);
	static bool addCsyFromInput(const Coverage* cov, Resource& res);
	static bool addGrfFromInput(const RasterCoverage* cov, Resource& resource);

    template<typename T> static bool execute(ExecutionContext* ctx, T func, const std::vector<IRasterCoverage>& rasters) {
        std::vector<BoundingBox> boxes;

		auto outputRaster = rasters.back(); // last entry is always the output raster and determines the core use.
		if (!outputRaster.isValid())
			return false;
		int cores = 1;
        ctx->_threaded = false;
		if (ctx->_threaded) {
			cores = QThread::idealThreadCount();
			int prefCount = outputRaster->grid()->blocks() / 2 + 1;
			if (prefCount < cores) {
				cores = prefCount;
			}


			for (auto raster : rasters) {
				if (raster.isValid()) {
					OperationHelperRaster::subdivideTasks(cores, raster, boxes);
					raster->gridRef()->prepare4Operation(cores);
				}
			}
		}
		else {
			boxes.push_back(BoundingBox(outputRaster->size()));
		}

       // std::vector<std::future<bool>> futures(cores);
        bool res = true;
        res = func(boxes[0],0);

        //2022-02-06 : for the moment disabled. for unknown reasons the std::async(std::launch::async, func, boxes[i],cores > 1 ? i + 1 : 0) statement crashes for all operations without every arriving at the function call.
        // At this moment I can not find any reason for this as nothing has changed in this respect. What the call std::async() is doing is quite complex with regards to moves and copies etc... so I suspect a compiler bug
        // as I have seen those before with regard to temporary objects being copied/moved but why now? Will investigate later

       // for(int i =0; i < cores; ++i) {
       //     futures[i] = std::async(std::launch::async, func, boxes[i],cores > 1 ? i + 1 : 0); // threadindex starts at 1 as the 0 is the default ( no threads). In the grid administration, the cache at index 0 is the standard cache used in the non threading case.
       // }

      //  for(int i =0; i < cores; ++i) {
       //     res &= futures[i].get();
      //  }
		for (auto raster : rasters)
			raster->gridRef()->unprepare4Operation();

        double minv=1e307,maxv = -1e307;
        double minBand = minv, maxBand = maxv;
        quint32 bandIx = 0;
        if ( res && outputRaster.isValid()) {
            if ( outputRaster->datadef().domain<>()->valueType() & itNUMBER) {
                PixelIterator iter(outputRaster);
                auto end = iter.end();
                while(iter != end){
                    double v = *iter;
                    minv = Ilwis::min(v,minv);
                    maxv = Ilwis::max(v,maxv);
                    minBand = Ilwis::min(v,minBand);
                    maxBand = Ilwis::max(v,maxBand);
                    ++iter;
                    if ( iter.zchanged()){
                        outputRaster->datadefRef(bandIx).range<NumericRange>()->max(maxBand);
                        outputRaster->datadefRef(bandIx).range<NumericRange>()->min(minBand);
                        minBand = 1e307;
                        maxBand = -1e307;
                        ++bandIx;
                    }
                }
                double resolution = outputRaster->datadefRef().range<NumericRange>()->resolution();
                NumericRange *rng = new NumericRange(minv, maxv, resolution);
                for(int i=0; i < outputRaster->size().zsize(); ++i){
                    outputRaster->datadefRef(i).range<NumericRange>()->resolution(resolution)    ;
                }
                outputRaster->datadefRef().range(rng);
            }
        }
        return res;
    }
    static IIlwisObject initialize(const IIlwisObject &inputObject, IlwisTypes tp, quint64 what);
};
}

#endif // OPERATIONHELPER_H
