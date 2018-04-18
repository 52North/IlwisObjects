#pragma once

#include <QObject>
#include <QQmlListProperty>
#include "visualpropertyeditor.h"

namespace Ilwis {
    namespace Ui {
        class CoverageLayerModel;

        class LinkCoverage : public VisualPropertyEditor
        {
            Q_OBJECT

        public:
            LinkCoverage();
            LinkCoverage(VisualAttribute *p);

            Q_INVOKABLE Ilwis::Ui::CoverageLayerModel *coverage();
            Q_INVOKABLE void setLink(const QVariantMap& parms);

            bool canUse(const IIlwisObject &obj, const QString &name) const;
            static VisualPropertyEditor *create(VisualAttribute *p);



            NEW_PROPERTYEDITOR(LinkCoverage)

        private:
        };
    }
}
