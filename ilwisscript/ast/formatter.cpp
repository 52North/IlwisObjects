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
#include "formatter.h"

using namespace Ilwis;

Formatter::Formatter() : ASTNode("Formatter")
{
}

void Formatter::setFormatNameSpace(IDNode *node)
{
    _namespace = QSharedPointer<IDNode>(node);
}

void Formatter::setFormat(char *s)
{
    QString quoted(s);
    _format = quoted.mid(1, quoted.size() - 2);
}

void Formatter::setDataType(const QString &tp)
{
    _dataType = tp;
}

QString Formatter::fnamespace() const
{
    if (_namespace.isNull())
        return sUNDEF;

    return _namespace->id();
}

QString Formatter::format() const
{
    return _format;
}

QString Formatter::dataType() const
{
    return _dataType;
}

//bool Formatter::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
//{
//    return false;
//}
