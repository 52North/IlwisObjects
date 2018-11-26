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

#ifndef OUTPARAMETERSNODE_H
#define OUTPARAMETERSNODE_H

namespace Ilwis {
class Selector;

class OutParametersNode : public ASTNode
{
public:
    OutParametersNode();
    void addSelector(const QString &id, Selector *n);
    void addResult(IDNode *n);
    void addSpecifier(const QString &id, ASTNode *n);
    QSharedPointer<Selector> selector(const QString &id) const;
    QString id(int index) const;
    QSharedPointer<ASTNode> specifier(const QString &id) const;

private:
    std::vector<QSharedPointer<IDNode>> _ids;
    std::map<QString,QSharedPointer<Selector>>  _selectors;
    std::map<QString,QSharedPointer<ASTNode>> _specifiers;
};
}

#endif // OUTPARAMETERSNODE_H
