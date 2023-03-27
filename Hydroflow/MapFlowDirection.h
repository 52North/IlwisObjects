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

#ifndef MAPFLOWDIRECTION_H
#define MAPFLOWDIRECTION_H

namespace Ilwis {
    namespace Hydroflow {

	class MapFlowDirection : public OperationImplementation
		{
		public:

			enum FlowMethod { fmSlope, fmHeight };
			
			MapFlowDirection();

			MapFlowDirection(quint64 metaid, const Ilwis::OperationExpression& expr);

			bool execute(ExecutionContext* ctx, SymbolTable& symTable);
			static Ilwis::OperationImplementation* create(quint64 metaid, const Ilwis::OperationExpression& expr);
			Ilwis::OperationImplementation::State prepare(ExecutionContext* ctx, const SymbolTable&);

			static quint64 createMetadata();


		FlowMethod m_fmMethods;
		bool       m_fParallel;
		

		private:
			IRasterCoverage _inRaster;
			IRasterCoverage _outRaster;
			IRasterCoverage _flagRaster;

			PixelIterator iterDEM;
			PixelIterator iterFlow;
			PixelIterator iterFlowFlag;

			IRasterCoverage _iterEmptyRaster;
			PixelIterator iterPos;


			long _xsize, _ysize;
			std::vector<Pixel> m_vFlat;	//store a continuous flat area  	
			std::vector<byte> m_vDirection;
			std::vector<byte> m_vFlowSelf;


		private:
			void executeFlowDirection();
			bool isInOneEdge(int iPos1, int iPos2, int iPos3, std::vector<int>& vPos);
			double rFindMaxLocation(std::vector<double>&, std::vector<int>&, int& iCout);
			long iLookUp(double rMax, int iCout, std::vector<int>&);

			void FillArray(Pixel pxl, std::vector<double>&);
			double rComputeSlope(double rCurH, double rNbH, int iPos);
			double rComputeHeightDifference(double rCurH, double rNbH);
			bool isEven(int elem);
			long m_ContFlat;
			void TreatFlatAreas();
			void LocateOutlets(Pixel rcInitFlat, std::vector<Pixel>&);
			void SetFlowsInFlatArea(std::vector<Pixel>& vOutlets);
			void InitPars();
			bool onEdge(Pixel pix);

            NEW_OPERATION(MapFlowDirection);
        };

	class Cell {
	public:
		Pixel pxl;
		byte val;
		int grd;
		Cell() {}
		Cell(Pixel pxl1) {
			pxl.x = pxl1.x;
			pxl.y = pxl1.y;
			pxl.z = pxl1.z;
		}
		void setRC(Pixel pxl1) {
			pxl.x = pxl1.x;
			pxl.y = pxl1.y;
			pxl.z = pxl1.z;
		}
	};


	class FlowDirectionAlgorithm 
	{
	public:
		enum Method { slope, height };
		enum FlowDirection { NW, N, NE, W, E, SW, S, SE };

	private:
		IRasterCoverage _inRaster;
		IRasterCoverage _flowdirRaster;
		IRasterCoverage _gradientRaster;

		PixelIterator iterDEM;
		PixelIterator iterFlow;

		IRasterCoverage _iterEmptyRaster;
		PixelIterator iterPos;

	private:
		FlowDirectionAlgorithm::Method method;
		byte Location[8];
		long lines, columns;
		byte increment;
		byte flatcell;
		byte flag;
		long _xsize, _ysize;


		Method methodValueOf(QString val);
		bool onEdge(Pixel pix);
		bool isEven(int elem);
		bool isInOneEdge(const std::vector<FlowDirection>& listPos, FlowDirection fd1, FlowDirection fd2, FlowDirection fd3);
		bool hasFlow(byte flowdirection);

		double maxAdj(Pixel pxl, double listVal[]);
		double maxAdj(Pixel pxl, PixelIterator gradient, double listVal[]);

		//
		void findDirection(double listA[], double val, std::vector<FlowDirection>& listPos);
		FlowDirection getFlowDirection(const std::vector<FlowDirection>& listPos);
		void locateOutlet(Pixel pxl, std::vector<Cell>& flatList, std::vector<Cell>& outList);
		void imposeGradient2LowerElevation( std::vector<Cell>& outletList, std::vector<Cell>& flatList, PixelIterator gradient);
		void imposeGradientFromElevation(std::vector<Cell>& flatList, PixelIterator gradient);
		void combineGradient( PixelIterator grd1, PixelIterator grd2, std::vector<Cell>& flatList);
		void assignFlowInFlat(std::vector<Cell>& flatList, PixelIterator gradient);
		void iniGradient(PixelIterator grd1, PixelIterator grd2, std::vector<Cell>& flatList);
		//
		double computeSlope(double h1,double h2,int pos);
		double computeHeightDifference(double h1,double h2);
		FlowDirection mapFlowLocation(int pos);
		//
	public:
		static int noflow;
			FlowDirectionAlgorithm(IRasterCoverage inRaster, IRasterCoverage flowdirRaster, IRasterCoverage emptyRaster);
			void calculate(QString method);
	};

    }
}

#endif // MAPFLOWDIRECTION_H

