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

#ifndef MAPFLOWACCEMULATION_H
#define MAPFLOWACCEMULATION_H

namespace Ilwis {
    namespace Hydroflow {

        class MapFlowAccumulation : public OperationImplementation
        {
        public:
			enum FillMethod { fmFill, fmCut };
            MapFlowAccumulation();

            MapFlowAccumulation(quint64 metaid, const Ilwis::OperationExpression& expr);

            bool execute(ExecutionContext* ctx, SymbolTable& symTable);
            static Ilwis::OperationImplementation* create(quint64 metaid, const Ilwis::OperationExpression& expr);
            Ilwis::OperationImplementation::State prepare(ExecutionContext* ctx, const SymbolTable&);

            static quint64 createMetadata();

        private:
			IRasterCoverage _inRaster;
            IRasterCoverage _outRaster;

            PixelIterator iterDEM;
            PixelIterator iterOut;

            long _xsize, _ysize;

        private:
            void executeFlowAccumulation();
            long iFlowAcc(Pixel pxl); //accumulated flow value for each cell 
   
            NEW_OPERATION(MapFlowAccumulation);
        };
    }
}

#endif // MAPFLOWACCEMULATION_H
