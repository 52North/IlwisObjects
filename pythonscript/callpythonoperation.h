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

#ifndef CALLPYTHONOPERATION_H
#define CALLPYTHONOPERATION_H

namespace Ilwis{

class OperationMetaData;
typedef IlwisData<OperationMetaData> IMetaData;

namespace PythonScript {
class CallPythonOperation : public OperationImplementation
{
public:
    CallPythonOperation();
    CallPythonOperation(quint64 metaid, const Ilwis::OperationExpression &expr);
    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);
    static quint64 createMetadata();

    static void addOperation(const QString& name, const QString& filename);

private:
    NEW_OPERATION(CallPythonOperation);

    static std::map<QString, QString> _pythonoperations;
    static bool _pathsAdded;
    QFileInfo _pythonFile;
    Resource _pythonMetadata;
};
}
}

#endif // CALLPYTHONOPERATION_H
