#pragma once

#include "kernel.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "chartoperationfactory.h"
#include "chartoperationeditor.h"

namespace Ilwis {
    namespace Ui {
        class ChartModel;

        class SetYAxisRange : public ChartOperationEditor
        {
            Q_OBJECT
        public:
            SetYAxisRange();

            bool canUse(ChartModel *model, const QVariantMap &parameter) const;

            Q_INVOKABLE void execute(const QVariantMap &parameters);
            static Ilwis::Ui::ChartOperationEditor *create() { return new SetYAxisRange(); }

            NEW_CHARTPROPERTYEDITOR(SetYAxisRange)
        };

    }
}
