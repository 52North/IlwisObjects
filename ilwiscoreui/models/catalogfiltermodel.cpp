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

#include "catalogfiltermodel.h"

using namespace Ilwis;
using namespace Ui;

CatalogFilterModel::CatalogFilterModel(QObject *parent) :
    QObject(parent),
    _selectionState(false),
    _query(sUNDEF),
    _name(sUNDEF),
    _readonly(true)
{
}

CatalogFilterModel::CatalogFilterModel(QObject *parent,const QString& query, const QString& name, const QString& icon, bool state) :
    QObject(parent),
    _selectionState(state),
    _query(query),
    _name(name),
    _icon(icon),
    _readonly(true)
{
}

CatalogFilterModel::CatalogFilterModel(QObject *parent, const QString &folder, const QString &name) :
    QObject(parent),
    _selectionState(true),
    _query("container='" + folder + "'"),
    _name(name),
    _url(folder),
    _readonly(true)
{

}

bool CatalogFilterModel::selectionState() const
{
    return _selectionState;
}

void CatalogFilterModel::setSelectionState(bool yesno)
{
    _selectionState = yesno;
    selectionStateChanged(_selectionState);
}

QString CatalogFilterModel::catalogQuery() const
{
    return _query;
}

void CatalogFilterModel::setCatalogQuery(const QString &stmt)
{
    _query = stmt;
    catalogQueryChanged(_query);
}

QString CatalogFilterModel::name() const
{
    return _name;
}

void CatalogFilterModel::setName(const QString &nm)
{
    _name = nm;
    nameChanged(_name);
}

QString CatalogFilterModel::icon() const
{
    return _icon;
}

void CatalogFilterModel::setIcon(const QString &icpath)
{
    _icon = icpath;
    iconChanged(_icon);
}

bool CatalogFilterModel::isReadOnly() const
{
    return _readonly;
}

void CatalogFilterModel::setReadOnly(bool yesno)
{
    _readonly = yesno;
    readOnlyChanged(_readonly);
}

QString CatalogFilterModel::url() const
{
    return _url;
}
