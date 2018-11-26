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

#include "menumodel.h"

using namespace Ilwis;
using namespace Ui;

MenuModel::MenuModel(QObject *parent) : QStandardItemModel(parent)
{
}

QHash<int, QByteArray> MenuModel::roleNames() const {
	return _roles;
}

void MenuModel::addRole(const QString& roleName) {
	int newId = Qt::UserRole + _roles.size() + 1;
	_roles.insert(newId, roleName.toLatin1());
	_reverseRoles[roleName] = newId;
}

QStandardItem * MenuModel::newItem() {
	QStandardItem *item = new QStandardItem();
	appendRow(item);
	return item;
}

int MenuModel::roleid(const QString& rolename) {
	if (_reverseRoles.find(rolename) != _reverseRoles.end())
		return _reverseRoles[rolename];

	return 0;
}