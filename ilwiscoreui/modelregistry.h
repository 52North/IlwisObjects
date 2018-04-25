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

        class ILWISCOREUISHARED_EXPORT ModelRegistry : public QObject
        {
            Q_OBJECT
                friend std::unique_ptr<ModelRegistry>& modelregistry();

        public:
            Q_PROPERTY(bool needsUpdate READ needsUpdate NOTIFY needsUpdateChanged)
            Q_INVOKABLE QVariantList modelList(quint32 selfId, const QString& types);
            Q_INVOKABLE QVariantList linkedProperties(int modelId);
            Q_INVOKABLE Ilwis::Ui::LayerManager* createLayerManager(QObject *parent, QQuickItem *viewContainer);
            Q_INVOKABLE Ilwis::Ui::TableModel *createTableModel(QObject *parent, const QString& url, const QString& type);
            Q_INVOKABLE Ilwis::Ui::ChartModel *createChartModel(QObject *parent);
            Q_INVOKABLE QObject* model(quint32 id);

            ModelRegistry(QObject *parent = 0);
            ~ModelRegistry();

            void registerModel(quint32 id, const QString& type, QObject *model);
            void unRegisterModel(quint32 id);
            std::pair<QString, QObject *> getModel(quint32) ;
            QString mainPanelUrl(const QString& type) const;

            quint32 newModelId();
            bool needsUpdate() const;
    
        signals:
            void needsUpdateChanged();

        private:
            std::map<quint32, std::pair<QString, QObject *>> _models;
            QString name(const QString& type, QObject *obj) const;

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

