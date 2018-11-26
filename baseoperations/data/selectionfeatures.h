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

#ifndef SELECTIONFEATURES_H
#define SELECTIONFEATURES_H

#include "geos/geom/geometry.h"
#include "geometries.h"
#include <memory>

namespace Ilwis {
namespace BaseOperations {

class SelectionFeatures : public SelectionBase
{
public:
    SelectionFeatures();
    SelectionFeatures(quint64 metaid, const Ilwis::OperationExpression &expr);
    ~SelectionFeatures();

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable&);

    static quint64 createMetadata();
private:



    bool _asIndex = false;


    NEW_OPERATION(SelectionFeatures);

 };
}
}

#endif // SELECTIONFEATURES_H
