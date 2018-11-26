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

#ifndef GENERATESCRIPTNODE_H
#define GENERATESCRIPTNODE_H

namespace Ilwis {
namespace PythonScript{

class GenerateScriptNode
{
public:
    GenerateScriptNode();
    GenerateScriptNode(const SPWorkFlowNode& node);

    std::vector<QString> execute(const OperationExpression& expression, const std::map<quint64, int>& idmap, QStringList& script, const QString &whitespace="");
    void clearCalculatedValues();

private:
    std::vector<QString> _parameterValues;
    SPWorkFlowNode _node;

    std::vector<QString> executeOperation(const OperationExpression& expression, const std::map<quint64, int>& idmap, QStringList& script, const QString& whitespace="");
    std::vector<QString> executeCondition(const OperationExpression& expression, const std::map<quint64, int>& idmap, QStringList& script);
    std::vector<QString> executeJunction(const OperationExpression& expression, const std::map<quint64, int>& idmap, QStringList& script);
    std::vector<QString> executeContent(const OperationExpression &expression, const std::map<quint64, int> &idmap, QStringList& script);
    std::vector<QString> executeLoop(const OperationExpression &expression, const std::map<quint64, int> &idmap, QStringList& script);
    bool next();
    QString parameterValue(int parmIndex) const;
};
}
}

#endif // GENERATESCRIPTNODE_H
