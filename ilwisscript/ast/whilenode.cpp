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
#include "ilwis.h"
#include "identity.h"
#include "astnode.h"
#include "operationnodescript.h"
#include "symboltable.h"
#include "expressionnode.h"
#include "whilenode.h"

using namespace Ilwis;

WhileNode::WhileNode()
{
}

void WhileNode::setCondition(ExpressionNode *expr)
{
    _condition = QSharedPointer<ExpressionNode>(expr);
}

QString WhileNode::nodeType() const
{
    return "whileStatement";
}

bool WhileNode::checkCondition(SymbolTable &symbols, int scope, ExecutionContext *ctx) {

    _condition->evaluate(symbols, scope, ctx);
    return _condition->value().toBool(0) == true;
}

bool WhileNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx) {
    if (_condition.isNull())
        return false;

    if ( !_condition->isValid())
        return false;

    while( checkCondition(symbols, scope, ctx)) {
        foreach(QSharedPointer<ASTNode> node, _childeren) {
            if ( node->nodeType() == "breakStatement") {
                node->evaluate(symbols, scope, ctx);
                if ( node->value().toBool(0) == false)
                    break;
            } else
                node->evaluate(symbols, scope, ctx);
        }
    }
    return true;
}
