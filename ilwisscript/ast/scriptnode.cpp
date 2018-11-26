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

using namespace Ilwis;

std::map<quint64, QSharedPointer<ASTNode> > ScriptNode::_activeFormat;

ScriptNode::ScriptNode()
{
}

QString ScriptNode::nodeType() const
{
    return "script";
}

Formatter * ScriptNode::activeFormat(IlwisTypes type)
{
    auto iter= _activeFormat.find(type);
    if ( iter != _activeFormat.end())
        return static_cast<Formatter *>((*iter).second.data());

    return 0;
}

void ScriptNode::setActiveFormat(quint64 tp, const QSharedPointer<ASTNode> &node)
{
    _activeFormat[tp] = node;
}
