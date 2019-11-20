#pragma once
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


namespace Ilwis {
	namespace RasterOperations {


		class ZonalStatisticsBase : public OperationImplementation
		{
		public:
			enum UndefHandling { uhIgnoreUndef1, uhIgnoreUndef2, uhIgnoreUndef, uhDontCare };
			ZonalStatisticsBase();

			ZonalStatisticsBase(quint64 metaid, const Ilwis::OperationExpression &expr);

			Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

			

		protected:
			enum Methods { eMAX, eMIN, eSUM, eAVERAGE};
			IRasterCoverage _inputRaster1;
			IRasterCoverage _inputRaster2;
			ITable _inputTable;
			int _zonalColumnIndex = iUNDEF;
			Methods _aggregationMethod;
			bool _needMapping = false;


			static void createMetadata(Ilwis::OperationResource &operation);
			void executeBase(std::unordered_map<quint32, std::pair<double, double>>& result);
		};

		class ZonalStatisticsTable : public ZonalStatisticsBase
		{
		public:
			ZonalStatisticsTable(quint64 metaid, const Ilwis::OperationExpression &expr);

			static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression& expr);
			bool execute(ExecutionContext *ctx, SymbolTable &symTable);

			static quint64 createMetadata();
			Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &st);

			NEW_OPERATION(ZonalStatisticsTable);

		private:
			ITable _outputTable;
		};

		class ZonalStatisticsRaster : public ZonalStatisticsBase
		{
		public:
			ZonalStatisticsRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

			static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression& expr);
			bool execute(ExecutionContext *ctx, SymbolTable &symTable);
			Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

			static quint64 createMetadata();

			NEW_OPERATION(ZonalStatisticsRaster);

		private:
			IRasterCoverage _outputRaster;
		};
	}
}



