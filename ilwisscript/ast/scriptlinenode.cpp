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

#include <map>
#include "ilwis.h"
#include "symboltable.h"
#include "astnode.h"
#include "idnode.h"
#include "formatter.h"
#include "scriptnode.h"
#include "scriptlinenode.h"

using namespace Ilwis;

ScriptLineNode::ScriptLineNode()
{
}

QString ScriptLineNode::nodeType() const
{
    return "scriptline";
}

bool ScriptLineNode::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx )
{
     _evaluated = true;
    foreach(QSharedPointer<ASTNode> node, _childeren) {
        if ( node->nodeType() == "formatnode") {
            Formatter *fnode = static_cast<Formatter *>(node.data());
            if ( fnode->dataType() == "gridcoverage")
                ScriptNode::setActiveFormat(itRASTER, node)   ;

        }
        if (!node->evaluate(symbols, scope, ctx)) {
            _evaluated =  false;
            break;
        }
    }
    // we do not keep al binary data in memory for rasters as within a script they might not get out of scope until
    // the script finishes. This quickly fills up memory. So we unload all binaries and the raster will reload when it
    // is needed (if it all). This means a slight performance hit but it is necessary
    symbols.unloadData();

    return _evaluated;
}
