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
#include "multiplicationnode.h"
#include "symboltable.h"
#include "commandhandler.h"

using namespace Ilwis;

MultiplicationNode::MultiplicationNode()
{
}

QString MultiplicationNode::nodeType() const
{
    return "mult";
}

bool MultiplicationNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
{
    if(!OperationNodeScript::evaluate(symbols, scope, ctx))
        return false;

    bool ret = true;
    foreach(RightTerm term, _rightTerm) {
        term._rightTerm->evaluate(symbols, scope, ctx) ;
        const NodeValue& vright = term._rightTerm->value();
        for(int i=0; i < vright.size(); ++i) {
            if ( term._operator == OperationNodeScript::oTIMES ){
                ret = handleTimes(i, vright, symbols, ctx);
            } else   if ( term._operator == OperationNodeScript::oDIVIDED ){
                ret = handleDiv(i, vright, symbols, ctx);
            }else   if ( term._operator == OperationNodeScript::oMOD ){
                ret = handleMod(i, vright, symbols, ctx);
            }

            if (!ret)
                return false;
        }
    }

    return ret;
}

bool MultiplicationNode::handleTimes(int index, const NodeValue& vright, SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var = resolveValue(index, _value, symbols);
    if ( SymbolTable::isNumerical(vright[index]) && SymbolTable::isNumerical(var)) {
        _value = {vright.toDouble(index) * var.toDouble(), NodeValue::ctNumerical};
        return true;
    }
    IlwisTypes used = typesUsed(index, vright, symbols);
    if ( hasType(used, itRASTER))
        return handleBinaryCases(index, vright, "binarymathraster", "times", symbols, ctx);
    if ( hasType(used, itTABLE))
        handleTableCases(index, vright, "binarymathtable", "times", symbols, ctx);
    return false;
}

bool MultiplicationNode::handleDiv(int index, const NodeValue& vright, SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var = resolveValue( index, _value, symbols);
    if (SymbolTable:: isNumerical(vright[index]) && SymbolTable::isNumerical(var)) {
        if ( vright.toDouble(index) == 0)
            return false;
        _value = {var.toDouble() /  vright.toDouble(index), NodeValue::ctNumerical};
        return true;
    }
    IlwisTypes used = typesUsed(index, vright, symbols);
    if ( hasType(used, itRASTER))
        return handleBinaryCases(index, vright, "binarymathraster", "divide", symbols, ctx);
    if ( hasType(used, itTABLE))
        return handleTableCases(index, vright, "binarymathtable", "divide", symbols, ctx);
    return false;
}

bool MultiplicationNode::handleMod(int index, const NodeValue& vright,SymbolTable &symbols, ExecutionContext *ctx) {
    QVariant var = resolveValue(index, _value, symbols);
    if ( SymbolTable::isIntegerNumerical(vright[index]) && SymbolTable::isIntegerNumerical(var)) {
        bool ok1=false, ok2=false;
       _value = {var.toInt(&ok1) %  vright.toInt(index, &ok2), NodeValue::ctNumerical};
       return ok1 && ok2;
    }
    IlwisTypes used = typesUsed(index, vright, symbols);
    if ( hasType(used, itRASTER))
        return handleBinaryCases(index, vright, "binarymathraster", "mod", symbols, ctx);
    if ( hasType(used, itTABLE))
        return handleTableCases(index, vright, "binarymathtable", "mod", symbols, ctx);
    return false;
}
