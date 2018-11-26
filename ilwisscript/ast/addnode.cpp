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

#include "kernel.h"
#include "errorobject.h"
#include "astnode.h"
#include "operationnodescript.h"
#include "addnode.h"
#include "symboltable.h"
#include "commandhandler.h"

using namespace Ilwis;

AddNode::AddNode()
{
}

QString AddNode::nodeType() const
{
    return "add";
}

bool AddNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
{
    if(!OperationNodeScript::evaluate(symbols, scope, ctx))
        return false;

    bool ret = true;
    foreach(RightTerm term, _rightTerm) {
        term._rightTerm->evaluate(symbols, scope, ctx) ;
        const NodeValue& vright = term._rightTerm->value();
        for(int i=0; i < vright.size(); ++i) {
            if ( term._operator == OperationNodeScript::oADD ){
                ret = handleAdd(i, vright, symbols, ctx);
            } else   if ( term._operator == OperationNodeScript::oSUBSTRACT ){
                ret = handleSubstract(i, vright, symbols, ctx);
            }
            if (!ret)
                return false;
        }
    }

    return ret;
}

bool AddNode::handleAdd(int index, const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var = resolveValue(index, _value, symbols);
    if ( SymbolTable::isNumerical(vright[index]) && SymbolTable::isNumerical(var)) {
       _value = {vright.toDouble(index) + var.toDouble(), NodeValue::ctNumerical};
       return true;
    }
    IlwisTypes used = typesUsed(index, vright, symbols);
    bool ok = false;
    if ( hasType(used, itRASTER))
        ok = handleBinaryCases(index, vright, "binarymathraster", "add", symbols, ctx);
    else if ( hasType(used , itFEATURE)){
        ok = handleBinaryCases(index, vright, "binarymathfeature", "add", symbols, ctx);

    } else if ( hasType(used,itTABLE)){
        ok = handleTableCases(index, vright, "binarymathtable", "add", symbols, ctx);

    }
    return ok;
}

bool AddNode::handleSubstract(int index, const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var = resolveValue(index, _value, symbols);
    if ( SymbolTable::isNumerical(vright[index]) && SymbolTable::isNumerical(var)) {
       _value = {var.toDouble() -  vright.toDouble(index), NodeValue::ctNumerical};
       return true;
    }
    IlwisTypes used = typesUsed(index, vright, symbols);
    bool ok = false;
    if ( hasType(used, itRASTER))
        ok = handleBinaryCases(index, vright, "binarymathraster", "subtract", symbols, ctx);
    else if (  (used & itFEATURE) == 0){
        ok = handleBinaryCases(index, vright, "binarymathfeature", "subtract", symbols, ctx);

    }
    return ok;
}



