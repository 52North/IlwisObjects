#include <QDebug>
#include <memory>
#include <qquickitem.h>
#include "kernel.h"
#include "ilwisdata.h"
#include "mastercatalog.h"
#include "iooptions.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "coverage.h"
#include "table.h"
#include "geometries.h"
#include "georeference.h"
#include "uicontextmodel.h"
#include "operationExpression.h"
#include "raster.h"
#include "layerinfoitem.h"
#include "representation.h"
#include "layermanager.h"
#include "intermediatelayermodel.h"
#include "rootlayermodel.h"

using namespace Ilwis;
using namespace Ui;

 std::map<QString, CreateLayer> LayerManager::_createLayers;
 quint32 LayerManager::_baseViewId = 0;

LayerManager::LayerManager()
{

}

LayerManager::LayerManager(QObject *parent, QQuickItem *viewContainer) : QObject(parent)
{
	_rootNode = new IntermediateLayerModel(this,this, "root","",IOOptions());
	_rootNode->nodeId(0); // by defintion
    _globalLayer =  new RootLayerModel(this, _rootNode);
	_rootNode->appendChild(_globalLayer);
    _tree = new TreeModel(_rootNode,this);
     _viewid = _baseViewId++;
	 _viewContainer = viewContainer;
	 // for the moment here; this must be moved to a better place; the static init method used often can not be used because factory and objects are in the same dll
	 if (_createLayers.find("compositelayer") == _createLayers.end()) {
		 //_createLayers["globallayermodel"] = RootLayerModel::create;
		 _createLayers["compositelayer"] = IntermediateLayerModel::create;

	 }
}

LayerManager::~LayerManager()
{
}

bool LayerManager::hasSelectionDrawer() const
{
    return _hasSelectionDrawer;
}

void LayerManager::setHasSelectionDrawer(bool yesno)
{
    _hasSelectionDrawer = yesno;
}

bool LayerManager::zoomInMode() const
{
	return _zoomInMode;
}

void LayerManager::setZoomInMode(bool yesno)
{
	_zoomInMode = yesno;
}

bool LayerManager::panningMode() const
{
	return _panningMode;
}

void LayerManager::setPanningMode(bool yesno)
{
	_panningMode = yesno;
}


void LayerManager::moveLayer(LayerModel *parentLayer, LayerModel *layer, LayerMovement type)
{
    if ( parentLayer){

        parentLayer->moveLayer(layer, type);
    }
    _globalLayer->moveLayer(layer, type);
}


void LayerManager::setLayerListName(const QString name)
{
    _layerListName = name;
}

QString LayerManager::layerListName() const
{
    return _layerListName;
}

TreeModel *LayerManager::layerTree()
{
	return _tree;
}

LayerModel *Ilwis::Ui::LayerManager::lastAddedCoverageLayer() const
{
	return _lastAddedCoverageLayer;
}

void Ilwis::Ui::LayerManager::lastAddedCoverageLayer(LayerModel * lyr)
{
	_lastAddedCoverageLayer = lyr;
}

void buildList(QList<LayerModel *>& list, LayerModel *parentItem) {

	if ( parentItem->isDrawable())
		list.push_back(parentItem);
	for (auto *TreeNode : parentItem->children()) {
		LayerModel *lm = TreeNode->as<LayerModel>();
		if (lm->isValid() )
			buildList(list, lm);
	}
}


QQmlListProperty<Ilwis::Ui::LayerModel> LayerManager::layerList()
{
	_layerList = QList<LayerModel *>();
	buildList(_layerList, _rootNode);
	return QQmlListProperty<LayerModel>(this, _layerList);
}

QList<Ilwis::Ui::LayerModel*> Ilwis::Ui::LayerManager::layerList2()
{
	if (_layerList.size() == 0)
		layerList();

	return _layerList;
}

LayerModel * LayerManager::findLayer(int nodeid)
{
	return static_cast<LayerModel *>(_tree->findNode(nodeid));
}


LayerModel *LayerManager::create(LayerModel *parentLayer, const ICoverage &cov, LayerManager *lm, const IOOptions &options)
{
    if (cov->coordinateSystem()->isUnknown() && lm->rootLayer()->screenCsy().isValid()){
        QString mes = QString("coordinate system 'unknown' not compatible with coordinate system of the layerview");
        kernel()->issues()->log(mes, IssueObject::itWarning);
        return 0;
    }
    if ( parentLayer == 0)
        parentLayer = lm->rootLayer();

    QString type = TypeHelper::type2name(cov->ilwisType());
    auto iter = _createLayers.find(type);
    if ( iter != _createLayers.end()){
        auto createFunc   = (*iter).second;
        QString layername = options.contains("layername") ? options["layername"].toString(): cov->name();
        LayerModel *layer = createFunc(lm, parentLayer,layername, cov->description(),options);
		if (!lm->rootLayer()->screenCsy().isValid()) {// first real layer sets the csy
			lm->rootLayer()->screenCsy(cov->coordinateSystem());
			lm->rootLayer()->viewEnvelope(cov->envelope());
			lm->rootLayer()->zoomEnvelope(cov->envelope());
		}
		layer->nodeId(lm->nextId());
		layer->fillData();
		lm->layerTree()->appendChild(parentLayer, layer);
		lm->lastAddedCoverageLayer(layer);

    
        return layer;
    }
    return 0;

}

int LayerManager::nextId() {
	return ++_nodeCounter;
}

LayerModel *LayerManager::create(LayerModel *parentLayer, const QString &type, LayerManager *lm, const QString& layername, const QString& description, const IOOptions &options)
{
    if ( parentLayer == 0)
        parentLayer = lm->rootLayer();
    if ( parentLayer){
        auto iter = _createLayers.find(type);
        if ( iter != _createLayers.end()){
            auto createFunc   = (*iter).second;
            LayerModel *layer = createFunc(lm, parentLayer,layername, description, options);
			layer->nodeId(lm->nextId());
			layer->fillData();
			lm->layerTree()->appendChild(parentLayer, layer);
            return layer;
        }
    }
    return 0;
}

int LayerManager::registerLayerModel(const QString &modelname, CreateLayer func)
{
    auto iter = _createLayers.find(modelname);
    if ( iter == _createLayers.end()){
        _createLayers[modelname] = func;
    }
    return 0;
}

void LayerManager::wholeMap()
{
	_globalLayer->zoomEnvelope(_globalLayer->viewEnvelope());
	QVariantMap mp;
	mp["x"] = 0;
	mp["y"] = 0;
	mp["z"] = 0;
	_globalLayer->cameraPosition(mp);
	_globalLayer->zoomFactor(1.0);
	_zoomInMode = false;

	refresh();
}

void LayerManager::refresh()
{
    emit layerListChanged();
	emit layerTreeChanged();
	needUpdate(true);
}

void Ilwis::Ui::LayerManager::needUpdate(bool yesno)
{
	_needUpdate = yesno;
	if (_needUpdate) // the signal shouldnt be sent in case of false because then the UI starts reacting when no reaction is needed
		emit needUpdateChanged();
}

bool Ilwis::Ui::LayerManager::needUpdate() const
{
	return _needUpdate;
}

void LayerManager::addCommand(const QString &expression)
{
    OperationExpression expr(expression);
    if ( expr.isValid()){
        ExecutionContext ctx;
        SymbolTable tbl;
        commandhandler()->execute(expression, &ctx, tbl);
    }
}

void LayerManager::clearLayers(LayerModel *parentLayer)
{
    if ( parentLayer == 0){
        _globalLayer->clearLayers();
        _hasSelectionDrawer = false;
    }
    parentLayer->clearLayers();
}

int Ilwis::Ui::LayerManager::layerCount()
{
	if (_layerList.size() == 0)
		layerList();
	return _layerList.count();
}

RootLayerModel *LayerManager::rootLayer() const
{
    return _globalLayer;
}

quint32 LayerManager::viewid() const
{
    return _viewid;
}

bool LayerManager::isValid() const
{
    if (!_globalLayer)
        return false;
    if ( !_globalLayer->screenCsy().isValid())
        return false;
    if ( !_globalLayer->screenGrf().isValid())
        return false;
    return true;
}

void LayerManager::viewArea(QQuickItem *area) {

	_viewContainer = area;
}

QQuickItem * LayerManager::viewArea() const
{
	return _viewContainer;
}







