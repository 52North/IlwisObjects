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

#include <functional>
#include <future>
#include "coverage.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "table.h"
#include "raster.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operationhelperfeatures.h"
#include "operation.h"
#include "transformcoordinates.h"

using namespace Ilwis;
using namespace FeatureOperations;

REGISTER_OPERATION(TransformCoordinates)

TransformCoordinates::TransformCoordinates()
{

}

TransformCoordinates::TransformCoordinates(quint64 metaid, const Ilwis::OperationExpression &expr): OperationImplementation(metaid,expr)
{

}

bool TransformCoordinates::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    return false;
}

OperationImplementation *TransformCoordinates::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new TransformCoordinates(metaid, expr);
}

quint64 TransformCoordinates::createMetadata()
{
    OperationResource operation({"ilwis://operations/transformcoordinates"});
    operation.setSyntax("transformcoordinates(inputfeaturemap, csydefintion)");
    operation.setDescription(TR("transforms the coordinates of all vertices of the features from one coordinate system to another"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itFEATURE,  TR("Source feature coverage"),TR("The feature coverage to be transformed to another coordinate system"));
    operation.addInParameter(1,itSTRING, TR("coordinate system definition"),TR("definition of new projection in terms of epsg or proj4 or the url of an existing coordinate system"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itFEATURE, TR("output feature coverage"), TR("output feature coverage where all vertices have new coordinates"));
    operation.setKeywords("features, vector, coordinatesystem,internal"); // internal for the moment as there is no implementation

    mastercatalog()->addItems({operation});
    return operation.id();
}

OperationImplementation::State TransformCoordinates::prepare(ExecutionContext *ctx, const SymbolTable &sym)
{
    OperationImplementation::prepare(ctx,sym);
    kernel()->issues()->log(TR("not implemented"));
    return sPREPAREFAILED;
}


