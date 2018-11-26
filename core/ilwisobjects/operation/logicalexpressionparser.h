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

#ifndef LOGICALEXPRESSIONPARSER_H
#define LOGICALEXPRESSIONPARSER_H

namespace Ilwis {

class LogicalExpressionPart {
public:
    LogicalExpressionPart(const QString& part);
    QString field() const;
    LogicalOperator condition() const;
    QString value() const;
    IlwisTypes valueType() const;
    LogicalOperator logicalConnector() const;
    void logicalConnector(LogicalOperator oper);
    bool isValid() const;
private:
    QString _field = sUNDEF;
    LogicalOperator _condition = loNONE;
    QString _value = sUNDEF;
    IlwisTypes _vt = itUNKNOWN;
    LogicalOperator _logicalOper = loNONE;


};

class LogicalExpressionParser
{
public:

    LogicalExpressionParser();
    LogicalExpressionParser(const QString& expr);
    bool parse(const QString& expr);
    bool isValid() const;
    std::vector<LogicalExpressionPart> parts() const;
private:
    std::vector<LogicalExpressionPart> _parts;


};
}

#endif // LOGICALEXPRESSIONPARSER_H
