#pragma once

#include <QObject>
#include <QQmlListProperty>
#include "visualpropertyeditor.h"

namespace Ilwis {
    namespace Ui {
        class StretchLimits : public VisualPropertyEditor
        {
            Q_OBJECT

                Q_PROPERTY(double min READ min WRITE min NOTIFY rangeChanged)
                Q_PROPERTY(double max READ max WRITE max NOTIFY rangeChanged)
                Q_PROPERTY(QVariantList markers READ markers NOTIFY markersChanged)

        public:
            StretchLimits();
            StretchLimits(VisualAttribute *p);

            bool canUse(const IIlwisObject &obj, const QString &name) const;

            static VisualPropertyEditor *create(VisualAttribute *p);

            NEW_PROPERTYEDITOR(StretchLimits)

            signals:
                void rangeChanged();
                void markersChanged();

        private:
            double min() const;;
            void min(double value);
            double max() const;
            void max(double value) const;
            QVariantList markers() const;
        };
    }
}