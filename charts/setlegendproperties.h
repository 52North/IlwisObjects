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

        class setLegendProperties : public ChartOperationEditor
        {
            Q_OBJECT
        public:
            setLegendProperties();

            bool canUse(ChartModel *model, const QVariantMap &parameter) const;

            Q_INVOKABLE void execute(const QVariantMap &parameters);
            static Ilwis::Ui::ChartOperationEditor *create() { return new setLegendProperties(); }

            NEW_CHARTPROPERTYEDITOR(setLegendProperties)
        };

    }
}
