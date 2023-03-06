//////////////////////////////////////////////////////////////////////
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

#ifndef MAPFILLSINKS_H
#define MAPFILLSINKS_H

namespace Ilwis {
    namespace Hydroflow {

        class MapFillSinks : public OperationImplementation
        {
        public:
			enum FillMethod { fmFill, fmCut };
            MapFillSinks();

            MapFillSinks(quint64 metaid, const Ilwis::OperationExpression& expr);

            bool execute(ExecutionContext* ctx, SymbolTable& symTable);
            static Ilwis::OperationImplementation* create(quint64 metaid, const Ilwis::OperationExpression& expr);
            Ilwis::OperationImplementation::State prepare(ExecutionContext* ctx, const SymbolTable&);

            static quint64 createMetadata();

        private:
			IRasterCoverage _inRaster;
            IRasterCoverage _outRaster;
            IRasterCoverage _flagRaster;

            PixelIterator iterDEM;
            PixelIterator iterOut;
            PixelIterator iterFlag;

            long _xsize, _ysize;
            long m_iFlag;
            std::deque<Pixel> _vPxlSinks;

            long m_sinkPixels;
            long m_sinkPixelsThreshold;
            double m_sinkHeight;
            FillMethod method;

			void executeFillSink();
            void SingleSinkFill();
            void FlagNeighbors(Pixel pxl);
            bool fLocateInitialSink(Pixel pxl);
            void FindSinkContArea(Pixel pxl);
            void FindSinkContArea2(Pixel pxl);
            void FlagAdjaCell(Pixel pxl, std::deque<Pixel>& vAdj);

            void GroupSinksFill();
            bool fIdentifyOutletCell(Pixel rcSink, Pixel& rcOutlet);
            bool IsPotentialOutlet(Pixel pxl);
            void FlatAreaFlag(Pixel rcOutlet);
            void DepresFill(Pixel rcOutlet);
            double getPixelValue(Pixel pxl);
            void setPixelValue(Pixel pxl, double val);
            void CutTerrain(Pixel rcOutlet);
            double getCutValue(Pixel rcOutlet);
            bool onEdge(Pixel pix);
            bool IsUndef(Pixel pxl);
            void GetNeighborCells(Pixel pxl, std::vector<double>& vNeighbors);


            NEW_OPERATION(MapFillSinks);
        };
    }
}

#endif // FILTERRASTER_H
