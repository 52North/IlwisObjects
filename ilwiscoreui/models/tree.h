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
