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

#ifndef EXPRESSIONNODE_H
#define EXPRESSIONNODE_H

namespace Ilwis{

typedef std::function<std::set<quint32>(const Indices& , const Indices& )> RelationFunc;

class ExpressionNode : public OperationNodeScript
{
public:
    ExpressionNode();
    QString nodeType() const;
    bool evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx);

private:
    bool handleAnd(int index,const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
    bool handleOr(int index,const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
    bool handleXor(int index,const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);

    template<typename Func> void handleIndexes(int index, const NodeValue& vright, Func fun)
    {
        Indices index1 = _value.value(index).value<Indices>();
        Indices index2 = vright.value(index).value<Indices>();
        std::set<quint32> resultSet = fun(index1, index2);
        Indices results(resultSet.begin(), resultSet.end());
        QVariant value;
        value.setValue<Indices>(results);
        _value = {value, Identity::newAnonymousName(), NodeValue::ctLIST};

    }

};
}

#endif // EXPRESSIONNODE_H
