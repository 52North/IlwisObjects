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
#include "modeller/model.h"
#include "workflow/modelbuilder.h"
#include "workflow/applicationmodelui.h"
#include "smcemodel.h"
#include "smcespatialalternatives.h"

using namespace Ilwis::Ui;

REGISTER_APPMODEL(SMCESpatialAlternatives, "smce")

using namespace Ilwis;

SMCESpatialAlternatives::SMCESpatialAlternatives()
{

}

SMCESpatialAlternatives::SMCESpatialAlternatives(ApplicationModel *app, QObject *parent) : ApplicationModelUI(app, parent)
{
    _panels["main"] = "SMCE/SMCESpatialAlternativePanel.qml";
    _panels["form"] = "SMCE/SMCEAppForm.qml";

}

ApplicationModelUI *SMCESpatialAlternatives::create(ApplicationModel *app, QObject *parent)
{
    return new SMCESpatialAlternatives(app, parent);
}

bool SMCESpatialAlternatives::execute(const QVariantMap &inputParameters, QVariantMap &outputParameters)
{
    return false;
}
