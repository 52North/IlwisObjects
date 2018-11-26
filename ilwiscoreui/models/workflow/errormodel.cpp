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

#include "errormodel.h"
#include <QString>
#include <QAbstractListModel>
#include <QDebug>

using namespace Ilwis;
using namespace Ui;

Message::Message(int id, QString msg)
{
    _id = id;
    _msg = msg;
}

Message::~Message()
{
}

int Message::id()
{
    return _id;
}

QString Message::msg()
{
    return _msg;
}


bool ErrorModel::instanceFlag = false;
ErrorModel* ErrorModel::_instance = NULL;
ErrorModel* ErrorModel::getInstance()
{
    if(! instanceFlag)
    {
        _instance = new ErrorModel();
        instanceFlag = true;
        return _instance;
    }
    else
    {
        return _instance;
    }
}

ErrorModel::~ErrorModel()
{
    instanceFlag = false;
}

QStringList ErrorModel::errorlist()
{
    return _errorlist;
}

void ErrorModel::addError(int id, QString msg)
{
    _errorlist.append(msg);
    emit errorlistChanged();
}

void ErrorModel::clearList()
{
    _errorlist.clear();
    emit errorlistChanged();
}

int ErrorModel::getCount()
{
    return _errorlist.count();
}
