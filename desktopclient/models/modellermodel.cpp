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
#include "modellermodel.h"

using namespace Ilwis;

ModellerModel::ModellerModel(QObject *parent) :
    QObject(parent)
{
    addWorkflow("GCL_INT__12.mpr", "Model of GCL_INT__12.mpr");
    addWorkflow("woredas.shp", "Model of woredas.shp");
    addWorkflow("average_monthly_temperature_june_7.mpr", "Model of average_monthly_temperature_june_7.mpr");
}

QMLModellerList ModellerModel::workflows()
{
    return  QQmlListProperty<WorkflowModel>(this, _models);
}

void ModellerModel::addWorkflow(const QString& name, const QString& description)
{
    _models.insert(_models.size(), new WorkflowModel(name, description, this));

}

qint64 ModellerModel::count()
{
    return _models.size();
}

WorkflowModel* ModellerModel::get(quint32 index)
{
    return _models[index];
}

void ModellerModel::removeWorkflow(quint32 index)
{
    if ( 0 <= index < _models.size()) {
        _models.removeAt(index);
    }
}

//---------------------------------------------------
WorkflowModel::WorkflowModel()
{

}

WorkflowModel::WorkflowModel(const QString &name, const QString &description, QObject *parent) : QObject(parent)
{
    _name = name;
    _description = description;
}

QString WorkflowModel::modelName() const
{
    return  _name;
}

QString WorkflowModel::name() const
{
    return  _name;
}

QString WorkflowModel::description() const
{
    return _description;
}
