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

#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

namespace Ilwis {
namespace BaseOperations {

class Assignment : public OperationImplementation
{
public:
    Assignment();
    Assignment(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable&);

    static quint64 createMetadata();
private:
    template<typename T> void setOutput(ExecutionContext *ctx,SymbolTable& symTable) {
        QVariant value;
        IlwisData<T> outputObj = _outputObj.as<T>();
        value.setValue<IlwisData<T>>(outputObj);
        QString name = outputObj->name();
        ctx->_results.push_back(name);
        Resource resource = outputObj->resource();
        symTable.addSymbol(_outputObj->name(),0, _outputObj->ilwisType(),value);
        if ( name.indexOf(ANONYMOUS_PREFIX) == -1 && resource.isValid()) {
            mastercatalog()->addItems({resource});
        }

    }
    IIlwisObject _inputObj;
    IIlwisObject _outputObj;
    double _number;
    QString _varName;

    bool assignRasterCoverage(ExecutionContext *ctx);
    bool assignFeatureCoverage(ExecutionContext *ctx);
    bool assignTable(ExecutionContext *ctx);

    NEW_OPERATION(Assignment);

};
}
}

#endif // ASSIGNMENT_H
