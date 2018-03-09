#include "kernel.h"
#include <vector>
#include "tree.h"

using namespace Ilwis;
using namespace Ui;

//---------------------------------------------------------------------------
Ilwis::Ui::TreeModel::TreeModel(QObject * parent) : QStandardItemModel(parent)
{
	/*_roleNameMapping[rName] = "name";
    _roleNameMapping[rIcon] = "icon";
    _roleNameMapping[rFixed] = "parenthasfixedstructure";
    _roleNameMapping[rNodeId] = "nodeid";
    _roleNameMapping[rActive] = "active";*/

    _roleNameMapping[Qt::UserRole + 1] = "data";
}



QHash<int, QByteArray> TreeModel::roleNames() const{
	return _roleNameMapping;
}




