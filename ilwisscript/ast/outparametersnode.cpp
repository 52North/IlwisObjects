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
#include "geometries.h"
#include "astnode.h"
#include "idnode.h"
#include "selectornode.h"
#include "outparametersnode.h"

using namespace Ilwis;

OutParametersNode::OutParametersNode()
{
}

void OutParametersNode::addSelector(const QString& id, Selector *n)
{
    _selectors[id]  = QSharedPointer<Selector>(n);
}

void OutParametersNode::addResult(IDNode *n)
{
    _ids.push_back(QSharedPointer<IDNode>(n));
}

void OutParametersNode::addSpecifier(const QString& id, ASTNode *n)
{
    _specifiers[id]  = QSharedPointer<ASTNode>(n);
}

QSharedPointer<Selector> OutParametersNode::selector(const QString& id) const
{
    auto iter = _selectors.find(id);
    if ( iter != _selectors.end())
        return (*iter).second;
    return QSharedPointer<Selector>();
}

QString OutParametersNode::id(int index) const
{
    if ( index < _ids.size())
        return _ids[index]->id();
    return sUNDEF;
}

QSharedPointer<ASTNode> OutParametersNode::specifier(const QString& id) const
{
    auto iter = _specifiers.find(id);
    if ( iter != _specifiers.end())
        return (*iter).second;
    return QSharedPointer<ASTNode>();
}


