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
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "operationmetadata.h"
#include "workflownode.h"
#include "workflow.h"
#include "analysispattern.h"
#include "applicationmodel.h"
#include "model.h"
#include "applicationmodelui.h"

using namespace Ilwis;
using namespace Ui;

ApplicationModelUI::ApplicationModelUI(QObject *parent) : QObject(parent)
{

}

ApplicationModelUI::ApplicationModelUI(Ilwis::ApplicationModel *app, QObject *parent) : QObject(parent), _app(app)
{

}

QString ApplicationModelUI::name() const
{
    if ( _app)    {
       return _app->name();
    }
    return "";
}

QString ApplicationModelUI::attachedAnalysis() const
{
    if ( _app)
        return _app->attachedAnalysis();
    return "";
}

void ApplicationModelUI::attachedAnalysis(const QString &name)
{
    if ( _app)
        _app->attachedAnalysis(name);
}

QString ApplicationModelUI::panel(const QString &panelName)
{
    auto iter = _panels.find(panelName);
    if ( iter != _panels.end()){
        return (*iter).second;
    }
    return "";
}
