/*IlwisObjects is a framework for analysis, processing and visualization of remote sensing and gis data
Copyright (C) 2018  52n North

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

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
            Q_INVOKABLE void setStretchLimit(const QString& attribute, double perc);
            Q_INVOKABLE void markersConfirmed();

            bool canUse(const IIlwisObject &obj, const QString &name) const;
			bool canUse(const IIlwisObject &, const DataDefinition &) const;

            static VisualPropertyEditor *create(VisualAttribute *p);

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
