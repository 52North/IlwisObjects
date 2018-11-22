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