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

#ifndef DRAINAGENETWORKORDERING_H
#define DRAINAGENETWORKORDERING_H

namespace geos {
	namespace geom {
		class Geometery;
	}
}

namespace Ilwis {
	namespace Hydroflow {

		typedef std::unique_ptr<geos::geom::Geometry> UPGeometry;

#define iDrainage -1
#define iOffDrainage -2 
#define iJunction 1
#define iInValidFlow 0

		struct RecordObj
		{
			RecordObj() {}
			long   id;
			QString UpstreamLink;
			Pixel  UpstreamCoord;
			long	 DownstreamLink;
			Pixel  DownstreamCoord;
			long   Strahler;
			long   Streve;
			double  Length;
			double  TotalUpstreamLength;
			Pixel  TostreamCoord;
		};

		class DrainageNetworkOrdering : public OperationImplementation
		{
		public:
			DrainageNetworkOrdering();
			DrainageNetworkOrdering(quint64 metaid, const Ilwis::OperationExpression& expr);

			bool execute(ExecutionContext* ctx, SymbolTable& symTable);
			static Ilwis::OperationImplementation* create(quint64 metaid, const Ilwis::OperationExpression& expr);
			Ilwis::OperationImplementation::State prepare(ExecutionContext* ctx, const SymbolTable&);

			static quint64 createMetadata();

		private:
			IRasterCoverage _inRaster;
			IRasterCoverage _inDrainageRaster;

			IRasterCoverage _outDrainageRaster;
			IRasterCoverage _inFldRaster;
			IRasterCoverage _iterEmptyRaster;

			IFeatureCoverage _outputfeatures;
			
			ITable _outputTable;
			ICoordinateSystem _csy;
			IGeoReference _inputgrf;

			INamedIdDomain _orderDomain;
			NamedIdentifierRange * _idrange;

			std::vector<Pixel>    m_vStreamLink; //store the location of cells in a link  	
			std::vector<Pixel>    m_vJunction;	//store the location of junctions in the network   	
			RecordObj         m_rec;
			std::vector<RecordObj> m_vRecords;

			std::vector<bool> tmpFeatureFlags;

			int usedIdCount;
			std::map<const geos::geom::Geometry*, int> lsidmap;

			long _xsize, _ysize;
			int _minlength;

			double	m_rMinLen;	//the minimum drainage length
			long	m_iLinkNumber; //link identification number
			double m_rLength;
			long   m_iStrahler;
			long   m_iStreve;
			double m_rTotalUpstreamLength;
			QString m_sUpstreamLink;

			std::vector<long> m_ids;
			bool isContinue(long id);

		private:
			void IniParms();
			bool IsEdgeCell(Pixel pxl);
			bool IsSourceCell(Pixel pxl);
			void FlagDrainageMap(long iFlag, int iLink);
			int  IdentifyLink(Pixel& pxl);
			double rDistance(Coordinate cd1, Coordinate cd2);
			bool fLatLonCoords();
			bool IsJunction(Pixel pxl);
			bool IsJunctionExists(Pixel node);
			int GetDownStreamCell(Pixel& pxl);
			long GetUpstream(Pixel pxl);
			bool executeDrainageNetworkOrdering();
			long FindDrainageIndex(long iDrainageID);
			void EraseJunction(Pixel pxl);
			void StoreJunction(Pixel pxl);
			geos::geom::CoordinateSequence* toCoordinateSequence(std::vector<Coordinate> cv);

			void CleanSegment(IFeatureCoverage smpTo, IFeatureCoverage smpFrom);
			void StoreSegment(IFeatureCoverage sm, long iSegID, bool fConvert);
			void PatchSegment(IFeatureCoverage sm, long iSegID);
			bool IsAllUpstreamIdentified(Pixel node, std::vector<long>& vUpstreamPos);
			void AddDomainItem(long iItem);
			long ComputeStrahlerOrder(std::vector<long> vUpstreamLink);
			long ComputeStreveOrder(std::vector<long> vUpstreamLink);
			double rComputeSlope(double rDrop, double rLength, bool fDegree);
			double rComputeSinuosity(double rLength, double rStraightLenght);

			void DownstreamLink(std::vector<long> vPos);
			void UpdateRecord();
			void CreateTable(long maxStrahler);
			void FillTableRecords();
			QString CoordinateFormatString(QString crd);

			NEW_OPERATION(DrainageNetworkOrdering);
		};
	
	}
		
}

#endif //DRAINAGENETWORKORDERING_H

