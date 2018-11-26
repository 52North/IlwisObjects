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

#ifndef SELECTNODE_H
#define SELECTNODE_H

namespace Ilwis {
class SelectNode : public ASTNode
{
public:
    SelectNode();

    void setInput(const QString &inputUrl);

    QString inputId() const;

    bool evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx);
    void setExpression(ExpressionNode *n);
private:
    QString _inputId = sUNDEF;
    QSharedPointer<ExpressionNode> _expression;
    bool _isUrl=false;
};
}

typedef std::vector<std::vector<Ilwis::Coordinate>> CoordinateLists;
Q_DECLARE_METATYPE(CoordinateLists);

#endif // SELECTNODE_H
