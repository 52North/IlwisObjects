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

#include "projectionparametermodel.h"

ProjectionParameterModel::ProjectionParameterModel()
{

}

ProjectionParameterModel::ProjectionParameterModel(const Ilwis::IProjection& proj, Ilwis::Projection::ProjectionParamValue type, QObject *parent) :
    QObject(parent)
{

   _name = Ilwis::Projection::parameterName(type);
   if ( proj->isUsed(type)){
        _value = proj->parameter(type).toString();
        _parmType = (int)type;
    }
   _valueType = proj->valueType(type);
}

ProjectionParameterModel::~ProjectionParameterModel()
{

}

QString ProjectionParameterModel::name() const
{
    return _name;
}

QString ProjectionParameterModel::value() const
{
    return _value;
}

QString ProjectionParameterModel::description() const
{
    return _description;
}

QString ProjectionParameterModel::valueType() const
{
   // return "aap";
    return Ilwis::TypeHelper::type2name(_valueType);
}

int ProjectionParameterModel::parameterType() const
{
    return _parmType;
}

//bool ProjectionParameterModel::isSet(const QString& type){
//    return proj->isSet(Ilwis::Projection::parameterName2type(type));
//}

