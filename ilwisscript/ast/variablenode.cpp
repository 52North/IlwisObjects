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

#include <QVariant>
#include <QRegExp>
#include "kernel.h"
#include "catalog.h"
#include "ilwisobject.h"
#include "astnode.h"
#include "idnode.h"
#include "operationnodescript.h"
#include "symboltable.h"
#include "expressionnode.h"
#include "variablenode.h"

using namespace Ilwis;

VariableNode::VariableNode(bool constant) : IDNode(0), _constant(constant)
{
}


QString VariableNode::nodeType() const
{
    return "Variable";
}

void VariableNode::setExpression(ExpressionNode * node)
{
    _expression = QSharedPointer<ExpressionNode>(node)    ;
}

bool VariableNode::evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx)
{
    QVariant var;
    if ( !_expression.isNull()) {
        _expression->evaluate(symbols, scope, ctx);
        var = _expression->value();
    }


    foreach(QSharedPointer<ASTNode> node, _childeren) {
        if ( node->nodeType() == "ID") {
            IDNode *idnode = static_cast<IDNode *>( node.data());
            idnode->evaluate(symbols, scope, ctx);
            IlwisTypes type = idnode->type();
            symbols.addSymbol(idnode->id(), scope,type, var);
        }
    }
    return true;
}


