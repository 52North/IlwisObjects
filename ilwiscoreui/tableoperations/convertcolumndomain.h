#ifndef CONVERTCOLUMNDOMAIN_H
#define CONVERTCOLUMNDOMAIN_H
#include "kernel.h"
#include "ilwisdata.h"
#include "tableoperation.h"

namespace Ilwis {
namespace Ui {
class TableModel;

class ILWISCOREUISHARED_EXPORT ConvertColumnDomain : public TableOperation
{
    Q_OBJECT
public:
    ConvertColumnDomain();

    bool canUse(TableModel *tblModel, const QVariantMap &parameter) const;

    Q_INVOKABLE void execute(const QVariantMap &parameters);
    static Ilwis::Ui::TableOperation *create() { return new ConvertColumnDomain(); }

};
}
}

#endif // CONVERTCOLUMNDOMAIN_H
