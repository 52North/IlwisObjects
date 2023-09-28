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

#ifndef MAPCATCHMENTEXTRACTION_H
#define MAPCATCHMENTEXTRACTION_H

namespace geos {
	namespace geom {
		class Geometery;
	}
}

namespace Ilwis {
	namespace Hydroflow {

#define sUpstreamLink  "UpstreamLinkID"
#define	sDownstreamLink  "DownstreamLinkID"
#define	sDownstreamCoord  "DownstreamCoord"

		struct AttCols
		{
			AttCols() {}
			long         DrainageID;
			std::vector<long> UpstreamLink;
			long	 DownstreamLink;
			Pixel DownstreamCoord;
		};

		struct AttUpstreamLink
		{
			AttUpstreamLink() {}
			long         DrainageID;
			std::vector<long> UpstreamLink;
		};

		class MapCatchmentExtraction : public OperationImplementation
		{
		public:
			MapCatchmentExtraction();
			MapCatchmentExtraction(quint64 metaid, const Ilwis::OperationExpression& expr);

			bool execute(ExecutionContext* ctx, SymbolTable& symTable);
			static Ilwis::OperationImplementation* create(quint64 metaid, const Ilwis::OperationExpression& expr);
			Ilwis::OperationImplementation::State prepare(ExecutionContext* ctx, const SymbolTable&);

			static quint64 createMetadata();

		private:
			bool executeCatchmentExtraction();
			bool IsEdgeCell(Pixel pxl);
			//void CompitableGeorefs(FileName fn, Map mp1, Map mp2);
			void GetAttributes();
			long DelineateCatchment(Pixel pxl, long iFlag);
			long FindDownstreamIndex(long DownstreamID);
			void UpdateUpstreamLinkID(long DrainageID, long UpstreamID);
			void EraseDrainage(long DrainageID);
			void ComputeCatchmentAttributes();
			bool fLatLonCoords();
			//void ComputeTotalUpstreamArea(DomainSort* pdsrt, Column cArea, Column cTotalArea);
			//void ComputerCenterPolygon(FileName fn);
			void ComputeCenterDrainage();
			double GetDistance(Pixel& rc);
			double rDistance(Coordinate cd1, Coordinate cd2);
			bool fEllipsoidalCoords();
			void SetAttributeTable();


			void SplitUpStreamLink(QString s, std::vector<long>& results);

		private:
			IRasterCoverage _inDrngOrderRaster;
			IRasterCoverage _inFldRaster;

			IRasterCoverage _outCatchmentRaster;
			IRasterCoverage _iterEmptyRaster;
			IRasterCoverage _flagRaster;
			
			IFeatureCoverage _outputfeatures;
			ITable _outputTable;

			ICoordinateSystem _csy;
			IGeoReference _inputgrf;

			long _xsize, _ysize;
			std::vector<AttCols> m_vRecords;

			NEW_OPERATION(MapCatchmentExtraction);
		};

	}

}

#endif //MAPCATCHMENTEXTRACTION_H


