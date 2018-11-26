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
#include "modeller/applicationmodel.h"
#include "modelsmcespatialalternatives.h"


using namespace Ilwis;
using namespace Smce;

REGISTER_MODELAPPLICATION(SMCESpatialAlternativesModel,"smcespatialalternatives")

SMCESpatialAlternativesModel::SMCESpatialAlternativesModel() :
    ApplicationModel("smcespatialalternatives",TR("calculates the result(s) of a specific smce for different areas"))
{
   attachedAnalysis("d4"); // a test name
}

void SMCESpatialAlternativesModel::store(QDataStream &stream)
{
   stream << type();
   ApplicationModel::store(stream);
}

void SMCESpatialAlternativesModel::loadMetadata(QDataStream &stream)
{
    ApplicationModel::load(stream);
}

void SMCESpatialAlternativesModel::loadData(QDataStream &stream)
{
    //TODO
}

QString SMCESpatialAlternativesModel::type() const
{
    return "smcespatialalternatives";
}

ApplicationModel *SMCESpatialAlternativesModel::create()
{
    return new SMCESpatialAlternativesModel()    ;
}
