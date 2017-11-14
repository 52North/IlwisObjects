#ifndef SORTCOLUMN_H
#define SORTCOLUMN_H

#include "kernel.h"
#include "ilwisdata.h"
#include "tableoperation.h"



namespace Ilwis {
namespace Ui {
class TableModel;

class SortColumn : public TableOperation
{
    Q_OBJECT


public:
    SortColumn();
    bool canUse(TableModel *tblModel, const QVariantMap &parameter) const;

    Q_INVOKABLE void execute(const QVariantMap &parameters);
    static Ilwis::Ui::TableOperation *create();
};
}
}

#endif // SORTCOLUMN_H
