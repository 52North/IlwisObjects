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
#include "analysismodel.h"

using namespace Ilwis;
using namespace Ui;

AnalysisModel::AnalysisModel(AnalysisPattern *pattern, QObject *parent) : QObject(parent), _analysis(pattern)
{

}

void AnalysisModel::suicide()
{
    delete this;
}

QString AnalysisModel::panel(const QString &panelName)
{
    auto iter = _panels.find(panelName);
    if ( iter != _panels.end()){
        return (*iter).second;
    }
    return "";
}

QString AnalysisModel::name() const{
    if ( _analysis)    {
       return _analysis->name();
    }
    return "";

}

QString AnalysisModel::longName() const
{
    if (resourceRef().hasProperty("longname"))
        return resourceRef()["longname"].toString();
    return "";
}

void AnalysisModel::longName(const QString &lname)
{
    resourceRef()["longname"]=lname;
}

Resource &AnalysisModel::resourceRef()
{
    if (_analysis && _analysis->attachedModel().isValid()) {
        return _analysis->attachedModel()->resourceRef();
    }
    throw ErrorObject(TR("trying to use uninitialized analysis model"));
}

const Resource &AnalysisModel::resourceRef() const
{
    if (_analysis && _analysis->attachedModel().isValid()) {
        return _analysis->attachedModel()->resourceRef();
    }
    throw ErrorObject(TR("trying to use uninitialized analysis model"));
}




