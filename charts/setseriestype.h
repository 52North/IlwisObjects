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

        class SetSeriesType : public ChartOperationEditor
        {
            Q_OBJECT
        public:
            SetSeriesType();

            bool canUse(ChartModel *model, const QVariantMap &parameter) const;

            Q_INVOKABLE void execute(const QVariantMap &parameters);
            static Ilwis::Ui::ChartOperationEditor *create() { return new SetSeriesType(); }

            NEW_CHARTPROPERTYEDITOR(SetSeriesType)
        };

    }
}
