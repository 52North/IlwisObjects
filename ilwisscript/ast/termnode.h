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

#ifndef TERMNODE_H
#define TERMNODE_H

namespace Ilwis {
class TermNode : public ASTNode
{
public:
    TermNode();
    void setNumerical(char *num);
    void setId(IDNode *node);
    void setExpression(ExpressionNode *n);
    void setString(char *s);
    void setParameters(ParametersNode *n);
    void setParameters();
    QString nodeType() const;
    void setLogicalNegation(bool yesno);
    void setNumericalNegation(bool yesno);
    bool evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx);
    void addSelector(Selector *n);

private:
    enum ContentState{csNumerical, csString, csExpression, csMethod,csID};
    double _number;
    QString _string;
    QSharedPointer<ExpressionNode> _expression;
    QSharedPointer<IDNode> _id;
    QSharedPointer<ParametersNode> _parameters;
    std::vector<QSharedPointer<Selector> > _selectors;
    ContentState _content;
    bool _logicalNegation;
    bool  _numericalNegation;

    QString getName(const Ilwis::NodeValue &var) const;
    QString buildBracketSelection(QString &name);
    bool doMethodStatement(SymbolTable &symbols, int scope, ExecutionContext *ctx);
    bool doIDStatement(SymbolTable &symbols, int scope, ExecutionContext *ctx);
    QString buildStatementSelection(ExecutionContext *ctx);
};
}

#endif // TERMNODE_H
