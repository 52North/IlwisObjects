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

#ifndef SELECTION_H
#define SELECTION_H


namespace Ilwis {
namespace BaseOperations {

class SelectionRaster : public SelectionBase
{
public:
    SelectionRaster();
    SelectionRaster(quint64 metaid, const Ilwis::OperationExpression &expr);
    ~SelectionRaster();

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable&);

    static quint64 createMetadata();
private:
    BoundingBox _box;

    NEW_OPERATION(SelectionRaster)    ;


};

class AttributeRaster : public SelectionRaster
{
public:
    AttributeRaster();
    AttributeRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable&);
    bool execute(ExecutionContext *ctx, SymbolTable &symTable);

    static quint64 createMetadata();

    NEW_OPERATION(AttributeRaster)    ;



};

class RasterBand : public SelectionRaster
{
public:
    RasterBand();
    RasterBand(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable&);
    bool execute(ExecutionContext *ctx, SymbolTable &symTable);

    static quint64 createMetadata();

    NEW_OPERATION(RasterBand)    ;



};
}
}
#endif // SELECTION_H
