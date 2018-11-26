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
#include "errorobject.h"
#include "symboltable.h"
#include "astnode.h"
#include "operationnodescript.h"
#include "expressionnode.h"
#include "ifnode.h"

using namespace Ilwis;

Ifnode::Ifnode() : ASTNode("IfNode")
{
}

bool Ifnode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
{
    if (_condition.isNull())
        throw Ilwis::ScriptSyntaxError(TR("uninitialized condition in 'if' statement"));
    _condition->evaluate(symbols, scope, ctx);
    if ( _condition->value().content() != NodeValue::ctBOOLEAN)
        throw Ilwis::ScriptSyntaxError(TR("'If' statement needs a boolean condition to function"));
    bool branch = _condition->value().toBool(0);
    bool ok = true;
    if ( branch) {
        for(quint32 i=0; i < _then.size(); ++i) {
            if ( _then[i].isNull()){
                kernel()->issues()->log("Then clause contains uninitialized statements", Ilwis::IssueObject::itWarning);
                continue;
            }

            ok &= _then[i]->evaluate(symbols, scope, ctx);
        }
    } else {
        for(quint32 i=0; i < _else.size(); ++i) {
            if ( _else[i].isNull()){
                kernel()->issues()->log("Else clause contains uninitialized statements", Ilwis::IssueObject::itWarning);
                continue;
            }

            ok &= _else[i]->evaluate(symbols, scope, ctx);
        }
        return ok;
    }
    return ok;
}

void Ifnode::setCondition(ExpressionNode *expr)
{
    _condition.reset(expr);
}

void Ifnode::addThen(ASTNode *node)
{
    _then.push_back(QSharedPointer<ASTNode>(node));
}

void Ifnode::addElse(ASTNode *node)
{
     _else.push_back(QSharedPointer<ASTNode>(node));
}
