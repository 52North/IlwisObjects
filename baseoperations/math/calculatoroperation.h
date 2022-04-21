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

#ifndef CALCULATOROPERATION_H
#define CALCULATOROPERATION_H

namespace Ilwis {
namespace BaseOperations{


class CalculatorOperation : public NumericOperation
{
public:
    enum ParmType {NUMERIC, ITERATOR,LINK, STRING, DOMAINITEM,COLUMN};
    enum MathAction{maIFF, maSIN, maCOS, maTAN, maASIN, maACOS,maATAN, maLOG10, maLN, maEXP,
                    maABS,maCEIL, maFLOOR,maSQ,maSQRT,maMAX,maMIN,maPOW,maADD,maMINUS,maDIVIDE,maMULT,
                    maEQ, maNEQ,maLESSEQ,maGREATEREQ,maLESS,maGREATER,maOR, maAND, maATTRIBUTE,maXOR,
                    maNOT,maIFUNDEF, maIFNOTUNDEF, maUNKNOWN};
    const int LEFT_ASSOC = 0;
    const int RIGHT_ASSOC = 1;




protected:
    struct ParmValue{

        ParmType _type = ITERATOR;
        PIXVALUETYPE _value = PIXVALUEUNDEF;
        int  _link = -1;
        PixelIterator *_source = 0; // for mapcalc
        QString _columName; // for tabcalc
        QString _string; // could be a string value or a colum name
        std::vector<QVariant> _columnValues;
		std::unordered_map<qint32, double> _keyMapping;
    };
    struct Action{
        std::vector<ParmValue> _values;
        MathAction _action;
    };

    std::map<QString, int> _functions;
    std::map<QString, std::vector<int>> _operators;
    std::map<int, PIXVALUETYPE> _inputNumbers;
    std::map<int,IDomain> _domains;
    std::vector<Action> _actions;
    int _record = 0;

    QStringList tokenizer(const QString &expr);
    CalculatorOperation();
    CalculatorOperation(quint64 metaid, const Ilwis::OperationExpression &expr);
    bool isAssociative(const QString &token, int type) ;
    bool isOperator(const QString &token) ;
    bool isNumber(const QString &token) const;
    bool isFunction(const QString &func);
    int cmpPrecedence(const QString &token1, const QString &token2) ;
    OperationImplementation::State  prepare(ExecutionContext *ctx,const SymbolTable&);
    QStringList shuntingYard(const QString &expr);
    CalculatorOperation::MathAction string2action(const QString &action);
    virtual void fillValues(int pindex, const QString &part, ParmValue &val, MathAction& _action) = 0;
    virtual bool check(int index) const = 0;
    virtual DataDefinition datadef(int index) = 0;
    IDomain collectDomainInfo(std::vector<std::vector<QString>>& rpn);
    IDomain linearize(const QStringList &tokens);
    PIXVALUETYPE  calc(const std::vector<Action>& localActions);
    int checkItem(int domainCount, QString &item, QString &copy_item, std::set<QString> &domainItems);
    int checkIndexItem(int domainCount, std::vector<std::vector<QString> > &rpn, std::vector<std::vector<QString> > &copy_rpn, int index, std::set<QString> &domainItems);
    void check(bool ok, const QString& error) const;

private:
    IDomain findOutDomain(const std::vector<std::vector<QString> > &rpn, const std::vector<QString> &node);
};
}
}

#endif // CALCULATOROPERATION_H
