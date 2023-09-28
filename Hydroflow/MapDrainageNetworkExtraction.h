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

#ifndef MAPDRAINAGENETWOREXTRACTION_H
#define MAPDRAINAGENETWOREXTRACTION_H

namespace Ilwis {
	namespace Hydroflow {

		class DrainageNetworkExtractionBase : public OperationImplementation
		{
		public:
			enum ThresholdMethod { tmValue, tmMap };
			DrainageNetworkExtractionBase();

			DrainageNetworkExtractionBase(quint64 metaid, const Ilwis::OperationExpression& expr);

			bool execute(ExecutionContext* ctx, SymbolTable& symTable);
			static Ilwis::OperationImplementation* create(quint64 metaid, const Ilwis::OperationExpression& expr);
			Ilwis::OperationImplementation::State prepare(ExecutionContext* ctx, const SymbolTable&);

			static quint64 createMetadata(OperationResource& operation);

		protected:
			IRasterCoverage _inRaster;
			IRasterCoverage _inThreshdRaster;
			IRasterCoverage _inFldRaster;
			IRasterCoverage _iterEmptyRaster;

			IRasterCoverage _outRaster;

			PixelIterator iterDEM;
			PixelIterator iterOut;
			PixelIterator iterPos;
			PixelIterator iterTshd;
			PixelIterator iterFld;

			long _xsize, _ysize;
			ThresholdMethod m_tmMethods;
			int m_iThreshold;

		protected:
			bool executeDrainageNetworkExtraction();
			bool IsEdgeCell(Pixel pxl);
			bool IsDeadendLine(Pixel& pxl);
		};

		class DrainageNetworkExtraction : public DrainageNetworkExtractionBase
		{

		public:
			DrainageNetworkExtraction(quint64 metaid, const Ilwis::OperationExpression& expr);

			static Ilwis::OperationImplementation* create(quint64 metaid, const Ilwis::OperationExpression& expr);
			Ilwis::OperationImplementation::State prepare(ExecutionContext* ctx, const SymbolTable&);

			static quint64 createMetadata();

			NEW_OPERATION(DrainageNetworkExtraction);
		};

		class DrainageNetworkExtractionWithStreamThreshold : public DrainageNetworkExtractionBase
		{

		public:

			DrainageNetworkExtractionWithStreamThreshold(quint64 metaid, const Ilwis::OperationExpression& expr);

			static Ilwis::OperationImplementation* create(quint64 metaid, const Ilwis::OperationExpression& expr);
			Ilwis::OperationImplementation::State prepare(ExecutionContext* ctx, const SymbolTable&);
			static quint64 createMetadata();

			NEW_OPERATION(DrainageNetworkExtractionWithStreamThreshold);
		};
	}
}

#endif //MAPDRAINAGENETWOREXTRACTION_H

