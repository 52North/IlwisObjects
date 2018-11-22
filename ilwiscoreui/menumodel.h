#pragma once

#include <QStandardItemModel>
#include <QHash>
#include "ilwiscoreui_global.h"

namespace Ilwis {
	namespace Ui {
		class ILWISCOREUISHARED_EXPORT MenuModel : public QStandardItemModel
		{
			Q_OBJECT
		public:
			MenuModel(QObject *parent=0);
			virtual QHash<int, QByteArray> roleNames() const;
			void addRole(const QString& roleName);
			QStandardItem *newItem();
			int roleid(const QString& rolename);

		private:
			QHash<int, QByteArray> _roles;
			std::map<QString, int> _reverseRoles;

		};
	}
}