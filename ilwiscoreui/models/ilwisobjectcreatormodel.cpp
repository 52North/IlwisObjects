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

#include "ilwisobjectcreatormodel.h"

using namespace Ilwis;
using namespace Ui;

IlwisObjectCreatorModel::IlwisObjectCreatorModel(QObject *parent) : QObject(parent)
{

}

IlwisObjectCreatorModel::IlwisObjectCreatorModel(const QString& name,const QString &label, quint64 type, const QString &creatorUrl, int height, QObject *parent):
QObject(parent),
  _name(name),
  _label(label),
  _type(type),
  _creatorUrl(creatorUrl),
  _height(height)
{

}

IlwisObjectCreatorModel::~IlwisObjectCreatorModel()
{

}

QString IlwisObjectCreatorModel::label() const
{
    return _label;
}

QString IlwisObjectCreatorModel::componentUrl() const
{
    return _creatorUrl;
}

QString IlwisObjectCreatorModel::type() const
{
    return QString::number(_type);
}

QString IlwisObjectCreatorModel::name() const
{
    return _name;
}

int IlwisObjectCreatorModel::height() const
{
    return _height;
}

