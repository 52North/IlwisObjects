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
#include "ilwisdata.h"
#include "domain.h"
#include "logicalexpressionparser.h"

using namespace Ilwis;

LogicalExpressionParser::LogicalExpressionParser()
{
}

LogicalExpressionParser::LogicalExpressionParser(const QString &expr)
{
    parse(expr);
}

bool LogicalExpressionParser::parse(const QString &ex)
{
    int index = 0;
    QString expr = ex.toLower();
    LogicalOperator operLast = loNONE;
    while(index != -1) {
        int shift = 0;
        LogicalOperator oper = loNONE;
        int newindex = expr.indexOf(" and ", index);
        if (newindex == -1){
            newindex = expr.indexOf(" or ", index);
            if ( newindex != -1){
                oper = loOR;
                shift = 3;
            }
        }else{
            oper = loAND;
            shift = 4;
        }
        LogicalExpressionPart part(ex.mid(index, newindex != -1 ? newindex : ex.size()));
        if ( !part.isValid()){
            _parts.clear();
            return ERROR2(ERR_ILLEGAL_VALUE_2,TR("expression"), ex);
        }
        part.logicalConnector(operLast);
        _parts.push_back(part);
        index = newindex + shift;
        operLast = oper;
    }
    return true;
}

bool LogicalExpressionParser::isValid() const
{
    return _parts.size() > 0;
}

std::vector<LogicalExpressionPart> LogicalExpressionParser::parts() const
{
    return _parts;
}

//---------------------------------------------------
LogicalExpressionPart::LogicalExpressionPart(const QString &ex)
{
    QString expr = ex.trimmed();
    typedef std::pair<QString, LogicalOperator> CondP;
    std::map<QString, LogicalOperator> conditions = {CondP("<=", loLESSEQ),CondP(">=", loGREATEREQ),
                                                     CondP("!=", loNEQ),CondP("==", loEQ),
                                                     CondP("<", loLESS),CondP(">", loGREATER)};
    for(auto condition : conditions) {
        int index = -1;
        if ( (index = expr.indexOf(condition.first)) != -1){
            QString left = expr.left(index);
            QString right = expr.mid(index + condition.first.size());
            _field = left.trimmed();
            _value = right.trimmed();
            _value = _value.remove('\"');
            _condition = condition.second;
            _vt = Domain::ilwType(_value);
            if ( _vt == itUNKNOWN)
                _vt = itSTRING;
            break;
        }
    }
}

QString LogicalExpressionPart::field() const
{
    return _field;
}

LogicalOperator LogicalExpressionPart::condition() const
{
    return _condition;
}

QString LogicalExpressionPart::value() const
{
    return _value;
}

IlwisTypes LogicalExpressionPart::valueType() const
{
    return _vt;
}

LogicalOperator LogicalExpressionPart::logicalConnector() const
{
    return _logicalOper;
}

bool LogicalExpressionPart::isValid() const
{
    return _field != sUNDEF && _condition != loNONE && _vt != itUNKNOWN;
}

void LogicalExpressionPart::logicalConnector(LogicalOperator oper)
{
    _logicalOper  = oper;
}
