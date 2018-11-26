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

#include "ilwis.h"
#include "symboltable.h"
#include "astnode.h"
#include "idnode.h"
#include "parametersnode.h"
#include "commandhandler.h"
#include "functionstatementnode.h"

using namespace Ilwis;

FunctionStatementNode::FunctionStatementNode(char *s) : IDNode(s)
{
}

void FunctionStatementNode::setParameters(ParametersNode *parm)
{
    _parameters = QSharedPointer<ParametersNode>(parm);
}

QString FunctionStatementNode::nodeType() const
{
    return "functionStatement";
}

bool FunctionStatementNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
{
    QString parms;
    if ( !_parameters.isNull()){
        for(int i=0; i < _parameters->noOfChilderen(); ++i) {
            bool ok = _parameters->child(i)->evaluate(symbols, scope, ctx);

            if (!ok)
                return false;
            auto node = _parameters->child(i)->value();
            QString name = node.toString();
            if ( name == sUNDEF)
                name = node.id();
            //QString name = getName(_parameters->child(i)->value());
            if ( parms.size() != 0)
                parms += ",";
            parms += name;

        }


    }
    QString exp = QString("%1(%2)").arg(id()).arg(parms);
    return commandhandler()->execute(exp,ctx, symbols);
}
