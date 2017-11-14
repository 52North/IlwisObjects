#ifndef COLUMNVISIBILITY_H
#define COLUMNVISIBILITY_H

#include "kernel.h"
#include "ilwisdata.h"
#include "tableoperation.h"



namespace Ilwis {
namespace Ui {
	class TableModel;
class ColumnVisibility : public TableOperation
{
    Q_OBJECT


public:
    ColumnVisibility();
    bool canUse(TableModel *tblModel, const QVariantMap &parameter) const;

    Q_INVOKABLE void execute(const QVariantMap &parameters);
    static Ilwis::Ui::TableOperation *create();
};
}
}

#endif // COLUMNVISIBILITY_H
