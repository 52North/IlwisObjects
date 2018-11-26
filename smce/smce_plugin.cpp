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

#include "smce_plugin.h"
//#include "geodrawer.h"

#include <qqml.h>

#include "kernel.h"
#include "ilwisdata.h"
#include "modeller/modellerfactory.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "workflownode.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"
#include "geometries.h"
#include "iooptions.h"
#include "smcemodel.h"
#include "smcespatialalternatives.h"

void SMCEPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<SMCEModel>(uri, 1, 0, "SMCEModel");
    qmlRegisterType<SMCESpatialAlternatives>(uri, 1, 0, "SMCESpatialAlternatives");
    qmlRegisterType<Node>(uri, 1, 0, "Node");
    qmlRegisterType<Weights>(uri, 1, 0, "Weights");
    qmlRegisterType<DirectWeightItem>(uri, 1, 0, "DirectWeightItem");
    qmlRegisterType<DirectWeights>(uri, 1, 0, "DirectWeights");
    qmlRegisterType<Standardization>(uri, 1, 0, "Standardization");
    qmlRegisterType<Anchor>(uri, 1, 0, "Anchor");
    qmlRegisterType<StandardizationValue>(uri, 1, 0, "StandardizationValue");
    qmlRegisterType<StandardizationValueConstraint>(uri, 1, 0, "StandardizationValueConstraint");
    qmlRegisterType<StandardizationClass>(uri, 1, 0, "StandardizationClass");
    qmlRegisterType<StandardizationBool>(uri, 1, 0, "StandardizationBool");
    qmlRegisterType<StandardizationBoolConstraint>(uri, 1, 0, "StandardizationBoolConstraint");

    Ilwis::kernel()->issues()->log("Loaded module SMCE",Ilwis::IssueObject::itMessage);
}


