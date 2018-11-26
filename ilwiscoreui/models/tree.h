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

#ifndef TESTTreeNode_H
#define TESTTreeNode_H

#include <QObject>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QVariant>
#include "ilwiscoreui_global.h"

namespace Ilwis{
namespace Ui{

class ILWISCOREUISHARED_EXPORT TreeModel : public QStandardItemModel
{
	Q_OBJECT

public:
    enum Roles { rName = Qt::UserRole + 1, rIcon = Qt::UserRole + 2, rFixed = Qt::UserRole + 3, rNodeId = Qt::UserRole + 4, rActive = Qt::UserRole + 5 };

    TreeModel(QObject *parent=0);

	QHash<int, QByteArray> roleNames() const override;
    


private:
	QHash<int, QByteArray> _roleNameMapping;
};


}
}

#endif // TESTTreeNode_H
