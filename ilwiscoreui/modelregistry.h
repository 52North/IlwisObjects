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
#include <QVariant>
#include <memory>
#include "ilwiscoreui_global.h"

class QQuickItem;

namespace Ilwis {
    namespace Ui {
        class TableModel;
        class ChartModel;
        class LayerManager;
        class LayerModel;

        class ILWISCOREUISHARED_EXPORT ModelRegistry : public QObject
        {
            Q_OBJECT
                friend std::unique_ptr<ModelRegistry>& modelregistry();

        public:
            Q_PROPERTY(bool needsUpdate READ needsUpdate NOTIFY needsUpdateChanged)
            Q_PROPERTY(quint32 lastAddedId READ lastAddedId WRITE lastAddedId NOTIFY lastAddedIdChanged)
            Q_INVOKABLE QVariantList modelList(QObject *source,  const QString& types);
            Q_INVOKABLE QVariantList linkedProperties(int modelId);
            Q_INVOKABLE Ilwis::Ui::LayerManager* createLayerManager(QObject *parent, QQuickItem *viewContainer);
            Q_INVOKABLE Ilwis::Ui::TableModel *createTableModel(QObject *parent, const QString& url, const QString& type);
            Q_INVOKABLE Ilwis::Ui::ChartModel *createChartModel(QObject *parent);
            Q_INVOKABLE QObject* model(quint32 id);
            Q_INVOKABLE QString mainPanelUrl(const QString& type) const;
            Q_INVOKABLE void unRegisterModel(quint32 id);


            ModelRegistry(QObject *parent = 0);
            ~ModelRegistry();

            void registerModel(quint32 id, const QString& type, QObject *model);
           ;
            std::pair<QString, QObject *> getModel(quint32) ;


            quint32 newModelId();
            bool needsUpdate() const;
            quint32 lastAddedId() const;
			void lastAddedId(quint64 id);
    
        signals:
            void needsUpdateChanged();
            void lastAddedIdChanged();

        private:
            std::map<quint32, std::pair<QString, QObject *>> _models;
            void getTargetContext(const QString& type, QObject * obj, const QString& targetType, QString& name, QString& url, quint64& id) const;
            quint32 _lastAddedId = 0;

            static std::unique_ptr<ModelRegistry> _modelregistry;
            static quint32 _baseId;
        };

        inline std::unique_ptr<Ilwis::Ui::ModelRegistry>& modelregistry() {
            if (Ilwis::Ui::ModelRegistry::_modelregistry.get() == 0) {
                Ilwis::Ui::ModelRegistry::_modelregistry.reset(new Ilwis::Ui::ModelRegistry());
            }
            return Ilwis::Ui::ModelRegistry::_modelregistry;
        }



    }
}

