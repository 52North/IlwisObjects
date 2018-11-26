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

#include <set>
#include "kernel.h"
#include "astnode.h"
#include "operationnodescript.h"
#include "symboltable.h"
#include "expressionnode.h"

using namespace Ilwis;

ExpressionNode::ExpressionNode()
{
}

QString ExpressionNode::nodeType() const
{
    return "expression";
}

bool ExpressionNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
{
    OperationNodeScript::evaluate(symbols, scope, ctx);
    const NodeValue& vleft = _leftTerm->value();
    _value = vleft;
    bool ret  = true;

    foreach(RightTerm term, _rightTerm) {
        term._rightTerm->evaluate(symbols, scope, ctx) ;
        const NodeValue& vright = term._rightTerm->value();
        for(int i=0; i < vright.size(); ++i) {
            if ( term._operator == OperationNodeScript::oAND ){
                ret = handleAnd(i,vright, symbols, ctx);
            } else   if ( term._operator == OperationNodeScript::oOR ){
                ret = handleOr(i,vright, symbols, ctx);
            } else   if ( term._operator == OperationNodeScript::oXOR ){
                ret = handleXor(i,vright, symbols, ctx);
            }
            if (!ret)
                return false;
        }
    }

    return ret;
}



bool ExpressionNode::handleAnd(int index, const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    if ( vright.canConvert(index,QVariant::Bool) && _value.canConvert(index,QVariant::Bool)) {
        _value =  {_value.toBool(index) &&  vright.toBool(index), NodeValue::ctBOOLEAN};
        return true;
    } else  if ( SymbolTable::isIntegerNumerical(vright) && SymbolTable::isIntegerNumerical(_value)){
        _value = {_value.toLongLong(index) & vright.toLongLong(index), NodeValue::ctNumerical};
        return true;
    } else if ( SymbolTable::isIndex(index, vright) && SymbolTable::isIndex(index, _value)){
        RelationFunc relation = [](const Indices& index1, const Indices& index2) {
            std::set<quint32> resultSet;
            for(auto i1 : index1){
                for(auto i2 : index2){
                    if ( i1 == i2)
                        resultSet.insert(i1);
                }
            }
            return resultSet;
        };
        handleIndexes(index, vright, relation);

        return true;
   }
   return handleBinaryCases(index, vright, "binarylogicalraster", "and", symbols, ctx);
}

bool ExpressionNode::handleOr(int index,const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    if ( vright.canConvert(index,QVariant::Bool) && _value.canConvert(index,QVariant::Bool)) {
        _value =  {_value.toBool(index) ||  vright.toBool(index), NodeValue::ctBOOLEAN};
        return true;
    } else  if ( SymbolTable::isIntegerNumerical(vright) && SymbolTable::isIntegerNumerical(_value)){
        _value = {_value.toLongLong(index) | vright.toLongLong(index), NodeValue::ctNumerical};
        return true;
    } else if ( SymbolTable::isIndex(index, vright) && SymbolTable::isIndex(index, _value)){
        RelationFunc relation = [](const Indices& index1, const Indices& index2) {
            std::set<quint32> resultSet;
            for(auto i1 : index1)
                resultSet.insert(i1);
            for(auto i2 : index2)
                resultSet.insert(i2);
            return resultSet;
        };
        handleIndexes(index, vright, relation);

        return true;
    }
    return handleBinaryCases(index, vright, "binarylogicalraster", "or", symbols, ctx);
}

bool ExpressionNode::handleXor(int index,const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    if ( vright.canConvert(index,QVariant::Bool) && _value.canConvert(index,QVariant::Bool)) {
        _value =  {_value.toBool(index) ^  vright.toBool(index), NodeValue::ctBOOLEAN};
        return true;
    } else  if ( SymbolTable::isIntegerNumerical(vright) &&SymbolTable::isIntegerNumerical(_value)){
        _value = {_value.toLongLong(index) ^ vright.toLongLong(index), NodeValue::ctNumerical};
        return true;
    } else if ( SymbolTable::isIndex(index, vright) && SymbolTable::isIndex(index, _value)){
        RelationFunc relation = [](const Indices& index1, const Indices& index2) {
            std::set<quint32> resultSet;
            std::map<quint32,quint32> combos;
            for(auto i1 : index1)
                combos[i1]++;
            for(auto i2 : index2)
                combos[i2]++;
            for(auto kvp : combos){
                if ( kvp.second == 1)
                    resultSet.insert(kvp.first);
            }
            return resultSet;
        };
        handleIndexes(index, vright, relation);

        return true;
    }
    return handleBinaryCases(index,vright, "binarylogicalraster", "xor", symbols, ctx);
}


