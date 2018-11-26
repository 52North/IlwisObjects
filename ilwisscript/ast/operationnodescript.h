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

#ifndef OPERATIONNODE_H
#define OPERATIONNODE_H

namespace Ilwis {
class OperationNodeScript : public ASTNode
{
public:
    enum Operators{oNONE, oADD, oSUBSTRACT, oMOD, oTIMES, oDIVIDED, oAND, oOR, oXOR, oLESS, oLESSEQ, oNEQ, oEQ, oGREATER, oGREATEREQ};
    struct RightTerm{
        QSharedPointer<ASTNode> _rightTerm;
        Operators _operator;
    };


    OperationNodeScript();
    void setLeftTerm(ASTNode *node);
    void addRightTerm(OperationNodeScript::Operators op, ASTNode *node);
    bool evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx);
    bool isValid() const;


protected:
    bool handleBinaryCases(int index, const NodeValue &vright, const QString& operation, const QString &relation,
                                   Ilwis::SymbolTable &symbols, ExecutionContext *ctx);
    bool handleTableCases(int index, const NodeValue &vright, const QString &operation, const QString &relation,
                          SymbolTable &symbols, ExecutionContext *ctx);
    IlwisTypes typesUsed(int index, const NodeValue &vright, SymbolTable &symbols) const;

    QSharedPointer<ASTNode> _leftTerm;
    QVector< RightTerm > _rightTerm;


private:
    QString additionalInfo(ExecutionContext *ctx, const QString &key) const;
};}

#endif // OPERATIONNODE_H
