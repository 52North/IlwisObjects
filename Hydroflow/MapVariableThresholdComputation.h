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

#ifndef VARIABLETHRESHOLDCOMPUTATION_H
#define VARIABLETHRESHOLDCOMPUTATION_H

namespace Ilwis {
    namespace Hydroflow {

        class VariableThresholdComputationBase : public OperationImplementation
        {
        public:
            VariableThresholdComputationBase();

            VariableThresholdComputationBase(quint64 metaid, const Ilwis::OperationExpression& expr);

            bool execute(ExecutionContext* ctx, SymbolTable& symTable);
            static Ilwis::OperationImplementation* create(quint64 metaid, const Ilwis::OperationExpression& expr);
            Ilwis::OperationImplementation::State prepare(ExecutionContext* ctx, const SymbolTable&);

            static quint64 createMetadata(Ilwis::OperationResource& operation);

        protected:
            struct ThresholdValues
            {
                double  UpperRelief;
                double  AccValue;
            };

            IRasterCoverage _inRaster;
            IRasterCoverage _outRaster;
            IRasterCoverage _reliefRaster;
            IRasterCoverage _iterEmptyRaster;

            PixelIterator iterDEM;
            PixelIterator iterOut;
            PixelIterator iterRelief;
            PixelIterator iterPos;

            std::vector<ThresholdValues> vThreshold;
            int m_iFilterSize;
            int m_iClasses;

            long _xsize, _ysize;
            QString inraster;

            bool executeThresholdComputation();
            bool executeReliefThresholdComputation();
            bool IsEdgeCell(Pixel pix);
            bool ComputeInternalRelief();
            bool ReClassify();
        };

        class InternalRelief : public VariableThresholdComputationBase
        {

        public:
            InternalRelief(quint64 metaid, const Ilwis::OperationExpression& expr);

            static Ilwis::OperationImplementation* create(quint64 metaid, const Ilwis::OperationExpression& expr);
            Ilwis::OperationImplementation::State prepare(ExecutionContext* ctx, const SymbolTable&);

            static quint64 createMetadata();

            NEW_OPERATION(InternalRelief);
        };

        class VariableThresholdComputation : public VariableThresholdComputationBase
        {
       
        public:

            VariableThresholdComputation(quint64 metaid, const Ilwis::OperationExpression& expr);

            static Ilwis::OperationImplementation* create(quint64 metaid, const Ilwis::OperationExpression& expr);
            Ilwis::OperationImplementation::State prepare(ExecutionContext* ctx, const SymbolTable&);
            static quint64 createMetadata();

            NEW_OPERATION(VariableThresholdComputation);
        };

    
    }
}

#endif // MAPVARIABLETHRESHOLDCOMPUTATION_H
