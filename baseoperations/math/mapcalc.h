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

#ifndef MAPCALC_H
#define MAPCALC_H


namespace Ilwis {
class ProcessingBoundingBoxes;
namespace BaseOperations{
class MapCalc : public CalculatorOperation
{
public:
    MapCalc();
    MapCalc(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);

    State prepare(ExecutionContext *ctx,const SymbolTable&);

private:
    std::map<int, PixelIterator> _inputRasters;

    IRasterCoverage _outputRaster;

    IDomain findOutDomain(const std::vector<std::vector<QString>>&rpn,const std::vector<QString>& node);
    void fillValues(int pindex, const QString &part, ParmValue &val, MathAction& _action);
    DataDefinition datadef(int index);
    bool check(int index) const;
	std::vector<IIlwisObject> rasters() const;
    void prepareActions(std::vector<Action>& localActions, std::map<int, PixelIterator>& inputRasters, const ProcessingBoundingBoxes& box, int threadIndex) const;
};

class MapCalc1 : public MapCalc{
public:
    MapCalc1(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(MapCalc1)    ;
};

class MapCalc2 : public MapCalc{
public:
    MapCalc2(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(MapCalc2)    ;
};

class MapCalc3 : public MapCalc{
public:
    MapCalc3(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(MapCalc3)    ;
};
class MapCalc4 : public MapCalc{
public:
    MapCalc4(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(MapCalc4)    ;
};

class MapCalc5 : public MapCalc{
public:
    MapCalc5(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(MapCalc5)    ;
};
class MapCalc6 : public MapCalc{
public:
    MapCalc6(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(MapCalc6)    ;
};
//class MapCalc7 : public MapCalc{
//public:
//    MapCalc7(quint64 metaid, const Ilwis::OperationExpression &expr);
//    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
//    static quint64 createMetadata();

//    NEW_OPERATION(MapCalc7)    ;
//};
//class MapCalc8 : public MapCalc{
//public:
//    MapCalc8(quint64 metaid, const Ilwis::OperationExpression &expr);
//    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
//    static quint64 createMetadata();

//    NEW_OPERATION(MapCalc8)    ;
//};
//class MapCalc9 : public MapCalc{
//public:
//    MapCalc9(quint64 metaid, const Ilwis::OperationExpression &expr);
//    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
//    static quint64 createMetadata();

//    NEW_OPERATION(MapCalc9)    ;
//};
}
}

#endif // MAPCALC_H
