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

#ifndef STRINGOPERATIONS_H
#define STRINGOPERATIONS_H

namespace Ilwis {
namespace BaseOperations {
class StringFind : public OperationImplementation
{
public:
    StringFind();

    StringFind(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &);

    static quint64 createMetadata();

private:
    QString _text;
    QString _searchText;
    qint32 _start=0;

    NEW_OPERATION(StringFind);
};

class StringSub : public OperationImplementation
{
public:
    StringSub();

    StringSub(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(StringSub);

private:
    QString _text;
    qint32 _end=10000000;
    qint32 _start=0;
};

class StringReplace : public OperationImplementation
{
public:
    StringReplace();

    StringReplace(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &);

    static quint64 createMetadata();

private:
    QString _text;
    QString _searchText;
    QString _replaceText;

    NEW_OPERATION(StringReplace);
};

class StringAdd : public OperationImplementation
{
public:
    StringAdd();

    StringAdd(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &);

    static quint64 createMetadata();

private:
    QString _text2;
    QString _text1;

    NEW_OPERATION(StringAdd);
};
}
}
#endif // STRINGOPERATIONS_H
