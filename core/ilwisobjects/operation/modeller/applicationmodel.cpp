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
#include "operationmetadata.h"
#include "symboltable.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "workflownode.h"
#include "workflow.h"
#include "applicationmodel.h"
#include "analysispattern.h"
#include "model.h"

using namespace Ilwis;

ApplicationModel::ApplicationModel()
{

}

ApplicationModel::ApplicationModel(const QString &name, const QString &description) : Identity(name,i64UNDEF,description)
{

}

void ApplicationModel::store(QDataStream &stream)
{
    Identity::store(stream)    ;
    stream << _analysisName;
}

void ApplicationModel::loadMetadata(QDataStream &stream)
{
    Identity::load(stream);
    stream >> _analysisName;
}

QString ApplicationModel::attachedAnalysis() const
{
    return _analysisName;
}

void ApplicationModel::attachedAnalysis(const QString &name)
{
    _analysisName = name;
}

void ApplicationModel::attachedModel(quint64 modelid)
{
    _modelId = modelid;
}

IModel ApplicationModel::attachedModel()
{
    if ( !_attachedModel.isValid())
        _attachedModel.prepare(_modelId);
    return _attachedModel;
}
