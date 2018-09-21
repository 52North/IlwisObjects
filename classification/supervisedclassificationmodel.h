#ifndef SCMODEL_H
#define SCMODEL_H

#include <QObject>
#include "workflow/analysismodel.h"

namespace Ilwis {
    namespace SClassification {
        class SupervisedClassificationmodel : public Ilwis::Ui::AnalysisModel
        {
            Q_OBJECT

        public:
            SupervisedClassificationmodel();
            SupervisedClassificationmodel(Ilwis::AnalysisPattern *p);
            static Ilwis::Ui::AnalysisModel *create(Ilwis::AnalysisPattern *pattern);
            Q_INVOKABLE QVariantMap execute(const QVariantMap parameters);

        private:

            NEW_ANALYSISMODEL

        };
    }
}

#endif // SCMODEL_H
