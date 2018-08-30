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
				Q_PROPERTY(double initStretchMin READ initStretchMin CONSTANT)
				Q_PROPERTY(double initStretchMax READ initStretchMax CONSTANT)
                Q_PROPERTY(int resolution READ resolution CONSTANT)
                Q_PROPERTY(double zoomLevel MEMBER _zoomLevel NOTIFY zoomLevelChanged)
                Q_PROPERTY(bool zoomOnPreset READ zoomOnPreset WRITE zoomOnPreset NOTIFY zoomOnPresetChanged)
                Q_PROPERTY(QVariantList markers READ markers NOTIFY markersChanged)

        public:
            StretchLimits();
            StretchLimits(VisualAttribute *p);

            Q_INVOKABLE void setMarkers(const QVariantList& marks);
            Q_INVOKABLE void setStretchLimit(double perc);
            Q_INVOKABLE void markersConfirmed();

            bool canUse(const IIlwisObject &obj, const QString &name) const;

            static VisualPropertyEditor *create(VisualAttribute *p);

            NEW_PROPERTYEDITOR(StretchLimits)

            signals:
                void rangeChanged();
                void markersChanged();
                void zoomLevelChanged();
                void zoomOnPresetChanged();

        private:
            double min() const;
            void min(double value);
            double max() const;
            void max(double value) const;
			double initStretchMin() const;
			double initStretchMax() const;
            QVariantList markers() const;
            int resolution() const;
            bool zoomOnPreset() const;
            void zoomOnPreset(bool onoff);

            bool _zoomOnPreset = false;
            double _zoomLevel;
        };
    }
}
