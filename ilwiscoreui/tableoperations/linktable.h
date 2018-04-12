#pragma once

#include "kernel.h"
#include "ilwisdata.h"
#include "tableoperation.h"



namespace Ilwis {
    namespace Ui {
        class TableModel;

        class LinkTable : public TableOperation
        {
            Q_OBJECT


        public:
            LinkTable();
            bool canUse(TableModel *tblModel, const QVariantMap &parameter) const;

            Q_INVOKABLE void execute(const QVariantMap &parameters);
            static Ilwis::Ui::TableOperation *create();
        };
    }
}