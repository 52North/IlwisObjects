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

#ifndef TABCALC_H
#define TABCALC_H


namespace Ilwis {
namespace BaseOperations{
class TabCalc : public CalculatorOperation
{
public:
    TabCalc();
    TabCalc(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);

    State prepare(ExecutionContext *ctx,const SymbolTable&);
private:
    std::map<int, QString> _inputColumns;
    ITable _inputTable;
    ITable _outputTable;
    QString _outputColumn;
    bool _createNewTable;


    Ilwis::IDomain collectDomainInfo(std::vector<std::vector<QString> >& rpn);
    IDomain findOutDomain(const std::vector<std::vector<QString>>&rpn,const std::vector<QString>& node);
    void fillValues(int pindex, const QString &part, ParmValue &val, MathAction& _action);
    virtual DataDefinition datadef(int index);
    bool check(int index) const;
    void preFetchColumnValues();
};

//--------------------------------------
class TabCalc1 : public TabCalc{
public:
    TabCalc1(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(TabCalc1)    ;
};

//--------------------------------------
class TabCalc2 : public TabCalc{
public:
    TabCalc2(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(TabCalc2)    ;
};

//--------------------------------------
class TabCalc3 : public TabCalc{
public:
    TabCalc3(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(TabCalc3)    ;
};

//--------------------------------------
class TabCalc4 : public TabCalc{
public:
    TabCalc4(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(TabCalc4)    ;
};

//--------------------------------------
class TabCalc5 : public TabCalc{
public:
    TabCalc5(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(TabCalc5)    ;
};
//--------------------------------------
class TabCalc6 : public TabCalc{
public:
    TabCalc6(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(TabCalc6)    ;
};
}
}

#endif // TABCALC_H
