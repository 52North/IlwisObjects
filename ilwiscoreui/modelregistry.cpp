#include "modelregistry.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "coverage.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "layermodel.h"
#include "feature.h"
#include "raster.h"
#include "coveragelayermodel.h"
#include "layermanager.h"
#include "chartmodel.h"
#include "tablemodel.h"
#include "modelregistry.h"

using namespace Ilwis;
using namespace Ui;

quint32 ModelRegistry::_baseId = 0;

std::unique_ptr<ModelRegistry> ModelRegistry::_modelregistry;

ModelRegistry::ModelRegistry(QObject *parent)
    : QObject(parent)
{
}

ModelRegistry::~ModelRegistry()
{
}

void ModelRegistry::registerModel(quint32 id, const QString& type, QObject * model)
{
    _models[id] = std::pair<QString, QObject *>(type, model);
    emit needsUpdateChanged();
}

void ModelRegistry::unRegisterModel(quint32 id)
{
    auto iter = _models.find(id);
    if (iter != _models.end()) {
        _models.erase(iter);
        emit needsUpdateChanged();
    }
}

std::pair<QString, QObject *> ModelRegistry::getModel(quint32 id) 
{
    auto iter = _models.find(id);
    if (iter != _models.end()) {
        return iter->second;
    }
    return std::pair<QString, QObject *>(sUNDEF, 0);
}

QString Ilwis::Ui::ModelRegistry::mainPanelUrl(const QString & type) const
{
    if (type == "coverage" || type == "featurecoverage" || type == "rastercoverage") {
            return CoverageLayerModel::mainPanelUrl();
    }
    if (type == "table" || type == "flattable" || type == "attributetable") {
        return TableModel::mainPanelUrl();
    }

    if (type == "layermanager") {
        return CoverageLayerModel::mainPanelUrl();
    }

    if (type == "chart") {
        return ChartModel::mainPanelUrl();
    }
    return sUNDEF;
}

QVariantList ModelRegistry::modelList(quint32 selfId, const QString & types)
{
    QStringList typeList = types.split("|");
    QVariantList result;
    for (auto item : _models) {
        if (item.first == selfId)
            continue;

        QString type = item.second.first;
        if (typeList.contains(type) || types == "all") {
            QString iobjectname = name(type, item.second.second);
            if (iobjectname != sUNDEF) {
                QVariantMap mp;
                mp["name"] = iobjectname;
                mp["modelid"] = item.first;
                result.push_back(mp);
            }
        }

    }
    return result;
}

QVariantList ModelRegistry::linkedProperties(int modelId) {
    std::pair<QString, QObject *> modelPair = getModel(modelId);
    QVariantList result;
    if (modelPair.first != sUNDEF) {
        QString type = modelPair.first;
        QObject *obj = modelPair.second;
        if (type == "coverage" || type == "featurecoverage" || type == "rastercoverage") {
            CoverageLayerModel *cmodel = dynamic_cast<CoverageLayerModel *>(obj);
            result.append(cmodel->linkProperties());
        }
        if (type == "table" || type == "flattable" || type == "attributetable") {
            TableModel *cmodel = dynamic_cast<TableModel *>(obj);
            result.append(cmodel->linkProperties());
        }
        if (type == "layermanager" ) {
            LayerManager *cmodel = dynamic_cast<LayerManager *>(obj);
            result.append(cmodel->linkProperties());
        }
        if (type == "chart") {
            ChartModel *cmodel = dynamic_cast<ChartModel *>(obj);
            result.append(cmodel->linkProperties());
        }
    }
    return result;
}

quint32 Ilwis::Ui::ModelRegistry::newModelId()
{
    return _baseId++;
}

bool Ilwis::Ui::ModelRegistry::needsUpdate() const
{
    return true;
}

QString ModelRegistry::name(const QString& type, QObject * obj) const
{
    if (type == "coverage" || type == "featurecoverage" || type == "rastercoverage") {
        CoverageLayerModel *cmodel = dynamic_cast<CoverageLayerModel *>(obj);
        QString name =  cmodel->coverage()->name();
        if (cmodel->layerManager()->managerType() == LayerManager::mtMAIN) {
            return name;
        }
    }
    if (type == "table" || type == "flattable" || type == "attributetable") {
        TableModel *cmodel = dynamic_cast<TableModel *>(obj);
        return cmodel->table()->name();
    }

    if (type == "layermanager") {
        LayerManager *cmodel = dynamic_cast<LayerManager *>(obj);
        return cmodel->layerListName();
    }

    if (type == "chart") {
        ChartModel *cmodel = dynamic_cast<ChartModel *>(obj);
        return cmodel->name();
    }
    return sUNDEF;
}

LayerManager *ModelRegistry::createLayerManager(QObject *parent, QQuickItem *viewContainer)
{
    LayerManager *manager = new LayerManager(parent, viewContainer);

    return manager;
}

Ilwis::Ui::TableModel *ModelRegistry::createTableModel(QObject *parent, const QString& filter, const QString& type)
{
    IlwisTypes tp = IlwisObject::name2Type(type);
    Resource resource;
    if (filter.indexOf("itemid=") != -1 || filter.indexOf("resource=") != -1) {
        std::vector<Resource> res = mastercatalog()->select(filter);
        if (res.size() != 1)
            return 0;
        resource = res[0];
    }
    else
        resource = mastercatalog()->name2Resource(filter, tp);
    if (resource.isValid()) {
        if (resource.extendedType() == itRASTER) {
            bool ok;
            quint64 rasterid = resource["rasterid"].toULongLong(&ok);
            if (ok) {
                IRasterCoverage raster;
                raster.prepare(rasterid);
                if (raster.isValid() && raster->attributeTable().isValid())
                    return new Ilwis::Ui::TableModel(raster->attributeTable(), parent);
            }
        }
        else {
            return new Ilwis::Ui::TableModel(resource, parent);
        }
    }
    return 0;
}

ChartModel *ModelRegistry::createChartModel(QObject *parent)
{
    ChartModel *chart = new ChartModel( parent);

    return chart;
}


