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
    _lastAddedId = id;
    emit needsUpdateChanged();
    emit lastAddedIdChanged();
}

void ModelRegistry::unRegisterModel(quint32 id)
{
    if (_models.size() == 0)
        return;

    auto iter = _models.find(id);
    if (iter != _models.end()) {
        _models.erase(iter);
        if (id == _lastAddedId) {
            _lastAddedId = iUNDEF;
            emit lastAddedIdChanged();
        }
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

    if (type == "minimalchart") {
        return ChartModel::minimalPanelUrl();
    }
    return sUNDEF;
}

QVariantList ModelRegistry::modelList(QObject *source, const QString & propertyType)
{


    QVariantList result;
    if (propertyType == "")
        return result;

    QString requiredTargetType;
    CoverageLayerModel *covLayer = dynamic_cast<CoverageLayerModel *>(source);
    quint64 ilwisid;
    if (covLayer)
        ilwisid = covLayer->coverage()->id();
    else {
        RootLayerModel *rootLayer = dynamic_cast<RootLayerModel *>(source);
        if (rootLayer) {
            ilwisid = rootLayer->layerManager()->modelId();
        }
        else {
            TableModel * tableModel = dynamic_cast<TableModel *>(source);
            if (tableModel) {
                ilwisid = tableModel->modelId();
            }
        }
    }
    if (propertyType == "zoomextent") {
        requiredTargetType = "layermanager";
    }  else if (propertyType == "stretch") {
        requiredTargetType = "rastercoverage";
    } else if (propertyType == "record") {
        requiredTargetType = "featurecoverage";
    }  else if (propertyType == "feature") {
        requiredTargetType = "table";
    }

    for (auto item : _models) {
        QString type = item.second.first;
        QString url,name;
        quint64 id = i64UNDEF;
        getTargetContext(type, item.second.second, requiredTargetType, url, name, id);
        if (id != i64UNDEF && id != ilwisid) {
            QVariantMap mp;
            mp["name"] = name;
            mp["modelid"] = item.first;
            bool found = false;
            for (auto item : result) { // should be done with std::find + lambda but fails on QVariant. no sure why
                if (item.toMap()["name"] == name) {
                    found = true;
                    break;
                }
            }
            if ( !found)
                result.push_back(mp);
        }
    }

    return result;
}

void ModelRegistry::getTargetContext(const QString& type, QObject * obj, const QString& targetType, QString& url, QString& name, quint64& id) const
{
    if (type == "coverage" || type == "featurecoverage" || type == "rastercoverage") {
        CoverageLayerModel *cmodel = dynamic_cast<CoverageLayerModel *>(obj);
        if (targetType.indexOf("coverage") != -1) {
            url = cmodel->url();
            id = cmodel->coverage()->id();
            name = cmodel->coverage()->name();
        } else if (targetType == "layermanager") {
            LayerManager *lm = cmodel->layerManager();
            if (lm->managerType() == LayerManager::mtMAIN) {
                url = "";
                name = lm->layerListName();
                if ( name != sUNDEF && name.toLower() != "country boundaries")
                    id = lm->modelId();
                
            }
        } else if (targetType == "record") {
            TableModel *tmodel = dynamic_cast<TableModel *>(obj);
            url = tmodel->url();
            name = tmodel->table()->name();
            if ( name != sUNDEF)
                id = tmodel->table()->id();
            
        }
    }
    if (type == "table" || type == "flattable" || type == "attributetable") {
        if (targetType.indexOf("coverage") != -1) {
            TableModel *tmodel = dynamic_cast<TableModel *>(obj);
            url = tmodel->url();
            name = tmodel->table()->name();
            if ( name != sUNDEF)
                id = tmodel->table()->id();
            
        }
    }

    if (type == "layermanager") {
        LayerManager *cmodel = dynamic_cast<LayerManager *>(obj);
        url = "";
        if (cmodel->managerType() == LayerManager::mtMAIN){
            name = cmodel->layerListName();
            if (name != sUNDEF && name.toLower() != "country boundaries" && cmodel)
                id = cmodel->modelId();
        }
    }

    if (type == "chart") {
        ChartModel *cmodel = dynamic_cast<ChartModel *>(obj);
        name =  cmodel->name();
        url = "";
        if ( name != sUNDEF)
            id = cmodel->modelId();
    }
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
        if (type == "rootlayer") {
            RootLayerModel *rmodel = dynamic_cast<RootLayerModel *>(obj);
            result.append(rmodel->layerManager()->linkProperties());
        }
    }
    return result;
}

quint32 ModelRegistry::newModelId()
{
    return _baseId++;
}

bool ModelRegistry::needsUpdate() const
{
    return true;
}

quint32 ModelRegistry::lastAddedId() const
{
    return _lastAddedId;
}

void ModelRegistry::lastAddedId(quint64 id) {
	_lastAddedId = id;
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
    ChartModel *chart = 0;
    if (parent == 0)
        chart = new ChartModel(this);
    else
       chart = new ChartModel( parent);

    return chart;
}

QObject * Ilwis::Ui::ModelRegistry::model(quint32 id)
{
    auto pair = getModel(id);
    if (pair.first != sUNDEF)
        return pair.second;

    return nullptr;
}


