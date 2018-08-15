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
#include "compositelayermodel.h"
#include "rootlayermodel.h"
#include "georefimplementation.h"
#include "undeterminedgeoreference.h"
#include "coveragelayermodel.h"
#include "modelregistry.h"
#include "tree.h"

using namespace Ilwis;
using namespace Ui;

 std::map<QString, CreateLayer> LayerManager::_createLayers;
 quint32 LayerManager::_baseViewId = 0;

LayerManager::LayerManager()
{

}

LayerManager::LayerManager(QObject *parent, QQuickItem *viewContainer) : QObject(parent)
{
    _tree = new TreeModel(this);
    _globalLayer =  new RootLayerModel(this, _tree->invisibleRootItem());
    _tree->invisibleRootItem()->appendRow(_globalLayer);
	 _viewContainer = viewContainer;
     _globalLayer->nodeId(nextId());
     _globalLayer->prepare(0);
     _modelId = modelregistry()->newModelId();
     _managerType = (viewContainer && viewContainer->objectName().indexOf("overview_mainui_") != -1) ? LayerManager::mtOVERVIEW : LayerManager::mtMAIN;
     modelregistry()->registerModel(modelId(), "layermanager", this);
	 // for the moment here; this must be moved to a better place; the static init method used often can not be used because factory and objects are in the same dll
	 if (_createLayers.find("compositelayer") == _createLayers.end()) {
		 //_createLayers["globallayermodel"] = RootLayerModel::create;
		 _createLayers["compositelayer"] = CompositeLayerModel::create;

	 }
}

LayerManager::~LayerManager()
{
    _postDrawers = QList<QObject *>();
    modelregistry()->unRegisterModel(modelId());
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

bool LayerManager::zoomOutMode() const
{
	return _zoomOutMode;
}

void LayerManager::setZoomOutMode(bool yesno)
{
	_zoomOutMode = yesno;
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

        //parentLayer->moveLayer(layer, type);
    }
   // _globalLayer->moveLayer(layer, type);
}


void LayerManager::setLayerListName(const QString name)
{
    _layerListName = name;
}

QString LayerManager::layerListName() const
{
    return _layerListName;
}

QStandardItemModel * Ilwis::Ui::LayerManager::layerTree()
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

QString LayerManager::layerData(const Coordinate & crdIn, const QString & attrName, QVariantList & items) 
{
	QString result;
    QStandardItem *rootItem = _tree->invisibleRootItem();
    for (int layerIndex = 0; layerIndex < rootItem->rowCount(); ++layerIndex) {
        LayerModel *layer = static_cast<LayerModel *>(rootItem->child(layerIndex));
		if (layer->isValid()) {
            QString txt = layer->layerData(crdIn, attrName, items);
			if (result != "" && txt != "")
				result += ";";
            result += txt;
		}
	}
	return result;
}

LayerModel * LayerManager::findLayer(int nodeid)
{
    QStandardItem *rootItem = _tree->invisibleRootItem();
    for (int layerIndex = 0; layerIndex < rootItem->rowCount(); ++layerIndex) {
        LayerModel *layer = static_cast<LayerModel *>(rootItem->child(layerIndex));
        if (layer->nodeId() == nodeid)
            return layer;
        layer = layer->findLayer(nodeid);
        if (layer)
            return layer;
    }
    return 0;
}

int LayerManager::nodeid(QModelIndex idx) const
{
    if (idx.isValid()) {
        // TreeNode *item = static_cast<TreeNode*>(index.internalPointer());
        QMap<int, QVariant> data = _tree->itemData(idx);
        if (data.size() >= 0) {
            QVariantMap mp = data[Qt::UserRole + 1].toMap();
            if (mp.contains("nodeid"))
                return mp["nodeid"].toInt();
        }
    }
    return -1;
}

void Ilwis::Ui::LayerManager::move(int nodeId, const QModelIndex & targetLocation)
{
    QStandardItem *currentLayer = findLayer(nodeId);
    auto startRow = currentLayer->index();
//    int shift = startRow.row()  < targetLocation.row() ? -1 : 0;
    QStandardItem *targetItem = _tree->itemFromIndex(targetLocation);

    QList<QStandardItem *> layers = _tree->takeRow(currentLayer->row());

//    _tree->insertRow(targetLocation.row() + shift, layers);
    _tree->insertRow(targetItem->row(), layers);


}


LayerModel *LayerManager::create(QStandardItem *parentLayer, const ICoverage &cov, LayerManager *lm, const IOOptions &options)
{
    if (cov->coordinateSystem()->isUnknown() && lm->rootLayer()->screenCsy().isValid()){
        QString mes = QString("coordinate system 'unknown' not compatible with coordinate system of the layerview");
        kernel()->issues()->log(mes, IssueObject::itWarning);
        return 0;
    }
    if (parentLayer == 0)
        parentLayer = lm->layerTree()->invisibleRootItem();

    QString type = TypeHelper::type2name(cov->ilwisType());
    auto iter = _createLayers.find(type);
    if ( iter != _createLayers.end()){
        auto createFunc   = (*iter).second;
        QString layername = options.contains("layername") ? options["layername"].toString(): cov->name();
        LayerModel *layer = createFunc(lm, parentLayer,layername, cov->description(),options);
		if (!lm->rootLayer()->screenCsy().isValid()) {// first real layer sets the csy
			lm->rootLayer()->screenCsy(cov->coordinateSystem());
            lm->rootLayer()->coverageEnvelope(cov->envelope());
            lm->rootLayer()->viewEnvelope(cov->envelope());
            if (cov->ilwisType() == itRASTER) {
                IRasterCoverage rc = cov.as<RasterCoverage>();
                if (rc->georeference()->grfType<UndeterminedGeoReference>()) {
                    Envelope envUndetermned(Coordinate(0, 0), Coordinate(rc->size().xsize(), rc->size().ysize())); // none.grf bounds
                    lm->rootLayer()->coverageEnvelope(envUndetermned);
                    lm->rootLayer()->viewEnvelope(envUndetermned);
                }
            }
		}
        else {
            // adjust rootlayer envelop to fit all layers
            Coordinate crd1_trans;
            Coordinate crd2_trans;
            Envelope envelop = cov->envelope();
            if (!cov->coordinateSystem()->isEqual(lm->rootLayer()->screenCsy().ptr())) {
                Coordinate crn1 = envelop.min_corner();
                Coordinate crn2 = envelop.max_corner();

                crd1_trans = lm->rootLayer()->screenCsy()->coord2coord(cov->coordinateSystem(), crn1);
                crd2_trans = lm->rootLayer()->screenCsy()->coord2coord(cov->coordinateSystem(), crn2);
            }
            else {
                crd1_trans = envelop.min_corner();
                crd2_trans = envelop.max_corner();
            }
            Envelope orgenv = lm->rootLayer()->coverageEnvelope();
            if (!(orgenv.contains(crd1_trans) && orgenv.contains(crd2_trans))) {
                orgenv += crd1_trans;
                orgenv += crd2_trans;

                lm->rootLayer()->coverageEnvelope(orgenv);
                if (cov->ilwisType() == itRASTER) {
                    // TODO: check georef none ?
                }
            }
        }
        qint32 lowernodeid = options.contains("lowernodeid") ? options["lowernodeid"].toInt() : iUNDEF;
		layer->nodeId(lm->nextId());
		layer->fillData();
        addLayer(parentLayer, layer, lm, lowernodeid);
     

		lm->lastAddedCoverageLayer(layer);

    
        return layer;
    }
    return 0;

}

void  LayerManager::addLayer(QStandardItem *parentLayer, LayerModel *layer, LayerManager *lm, int lowernodid) {
    bool added = false;
    for (int childIndex = 0; childIndex < parentLayer->rowCount(); ++childIndex) {
        LayerModel * childLayer = static_cast<LayerModel *>(parentLayer->child(childIndex)); 
        if (layer->order() == iUNDEF) { // if the to be added layer has no predefined order, basically a normal data layer
            if (lowernodid != iUNDEF )  { // if the position to be added has an order
                if (childLayer->nodeId() == lowernodid) { // see if the to be added layer has the same number
                    lm->layerTree()->insertRow(childLayer->row(), layer); // if so it will be added above the child; basically an apped below all the data layers
                    added = true;
                }
            } else if (childLayer->order() != iUNDEF) { // if the current child has an order, we already know that the added layer has no order, append the layer below the data layers
                if (parentLayer->index().isValid()) { // the index of the top parent ,invisible root, is by definition invalid. We know that there is always a forground layer at level 0 so we must insert at 1 in that case
                    parentLayer->insertRow(0, layer);
                }else
                    parentLayer->insertRow(1, layer);
                 added = true;
            } 
        }
        else {
            if (childLayer->order() <= layer->order()) { // insert at top
                lm->layerTree()->insertRow(childLayer->row(), layer);
                added = true;
            }
        }
        if (added)
            break;
    }
    if (!added) // insert at bottom
        parentLayer->appendRow(layer);

    emit lm->allCoveragesChanged();
    emit lm->updateAxis();

}

QQmlListProperty<LayerModel> Ilwis::Ui::LayerManager::childLayersPrivate()
{
    _childeren = QList<LayerModel *>();
    QStandardItem *root = _tree->invisibleRootItem();
    for (int layerIndex = 0; layerIndex < root->rowCount(); ++layerIndex) {
        LayerModel *lyr = static_cast<LayerModel *>(root->child(layerIndex));
        _childeren.push_back(lyr);
    }
    return QQmlListProperty<LayerModel>(this, _childeren);
}

void getCoverages(QStandardItem *parent, QList<LayerModel *>& coverages) {
    for (int layerIndex = 0; layerIndex < parent->rowCount(); ++layerIndex) {
        LayerModel *layer = static_cast<LayerModel *>(parent->child(layerIndex));
        CoverageLayerModel *coverageLayer = dynamic_cast<CoverageLayerModel *>(layer);
        if (coverageLayer) {
            coverages.push_back(coverageLayer);
        }
        getCoverages(layer, coverages);
    }
}
QQmlListProperty<LayerModel> LayerManager::allCoveragesPrivate()
{
    _coverages = QList<LayerModel *>();
    QStandardItem *root = _tree->invisibleRootItem();
    getCoverages(root, _coverages);

    return QQmlListProperty<LayerModel>(this, _coverages);
 
}

QQmlListProperty<QObject> LayerManager::postDrawers() {
    return QQmlListProperty<QObject>(this, _postDrawers);

}

bool LayerManager::updatePostDrawersPrivate()
{
    return true;
}

int LayerManager::nextId() {
	return _nodeCounter++;
}

quint32 Ilwis::Ui::LayerManager::modelId() const
{
    return _modelId;
}

void LayerManager::setSelectionPrivate(const Coordinate& crd, LayerModel * layer) {
    CoverageLayerModel *coverageLayer = dynamic_cast<CoverageLayerModel *>(layer);
    if (coverageLayer) {
        Coordinate coord = crd;
        if (coverageLayer->coverage()->coordinateSystem() != rootLayer()->screenCsy()) {
            coord = coverageLayer->coverage()->coordinateSystem()->coord2coord(rootLayer()->screenCsy(), crd);
        }
        QVariant var = coverageLayer->coverage()->coord2value(coord);
        QVariantMap mp = var.toMap();
        if (!mp.isEmpty()) {
            bool ok;
            quint64 featureid = mp[FEATUREIDDCOLUMN].toULongLong(&ok);
            if (ok) {
                coverageLayer->addSelection(featureid, true);
                QVariantMap data;
                data["identity"] = featureid;
                data["ilwisobjectid"] = coverageLayer->coverage()->id();
                data["ilwistype"] = coverageLayer->coverage()->ilwisType();
                data["index"] = mp["index"].toUInt();
                coverageLayer->sendLink(data);
            }

        }
    }
    for (int layerIndex = 0; layerIndex < layer->rowCount(); ++layerIndex) {
        LayerModel *childLayer = static_cast<LayerModel *>(layer->child(layerIndex));
        setSelectionPrivate(crd, childLayer);
    }
}
void LayerManager::setSelection(const QString & pixelpair)
{
    QStringList parts = pixelpair.split("|");
    if (parts.size() == 2) {
        Ilwis::Coordinate crd = rootLayer()->screenGrf()->pixel2Coord(Ilwis::Pixel(parts[0].toDouble(), parts[1].toDouble()));
        QStandardItem *root = _tree->invisibleRootItem();
        for (int layerIndex = 0; layerIndex < root->rowCount(); ++layerIndex) {
            LayerModel *layer = static_cast<LayerModel *>(root->child(layerIndex));
            setSelectionPrivate(crd, layer);

        }

    }
}

QVariantList Ilwis::Ui::LayerManager::linkProperties() const
{
    QVariantList result;
    QVariantMap mp;
    mp["name"] = "zoom";
    mp["modelid"] = modelId();
    result.push_back(mp);

    return result;
}

LayerManager::ManagerType LayerManager::managerType() const
{
    return _managerType;
}

void Ilwis::Ui::LayerManager::updateAxis() 
{
    emit axisValuesChanged();
}

LayerModel *LayerManager::create(QStandardItem *parentLayer, const QString &type, LayerManager *lm, const QString& layername, const QString& description, const IOOptions &options)
{
    if ( parentLayer == 0)
        parentLayer = lm->layerTree()->invisibleRootItem();
    if ( parentLayer){
        auto iter = _createLayers.find(type);
        if ( iter != _createLayers.end()){
            auto createFunc   = (*iter).second;
            LayerModel *layer = createFunc(lm, parentLayer,layername, description, options);
			layer->nodeId(lm->nextId());
            layer->fillData(); 
            qint32 lowernodeid = options.contains("lowernodeid") ? options["lowernodid"].toInt() : iUNDEF;
            addLayer(parentLayer, layer, lm,lowernodeid);
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
	_globalLayer->viewEnvelope(_globalLayer->coverageEnvelope());
}

void LayerManager::refresh()
{
	emit layerTreeChanged();
    emit axisValuesChanged();
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

RootLayerModel *LayerManager::rootLayer() const
{
    return _globalLayer;
}

quint32 LayerManager::viewid() const
{
    return modelId();
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

void LayerManager::linkAcceptMessage(quint32 id, const QString& sourceType, const QVariantMap& parameters) {
}

QVariantList LayerManager::xGridAxisTop() const
{
    if (rootLayer()) {
        return rootLayer()->gridAxis("xaxisvaluestop");
    }

    return QVariantList();
}

QVariantList LayerManager::xGridAxisBottom() const
{
    if (rootLayer()) {
        return rootLayer()->gridAxis("xaxisvaluesbottom");
    }

    return QVariantList();
}

QVariantList LayerManager::yGridAxisLeft() const
{
    if (rootLayer()) {
        return rootLayer()->gridAxis("yaxisvaluesleft");
    }

    return QVariantList();
}

void LayerManager::addPostDrawer(QObject* editor)
{
    auto iter = std::find(_postDrawers.begin(), _postDrawers.end(), editor);
    if (iter == _postDrawers.end()) {
        _postDrawers.append(editor);
    }
}

void LayerManager::removePostDrawer(QObject * editor)
{
    auto iter = std::find(_postDrawers.begin(), _postDrawers.end(), editor);
    if (iter != _postDrawers.end()) {
        _postDrawers.erase(iter);
    }
}

void LayerManager::updatePostDrawers()
{
    emit updatePostDrawersChanged();
}

QVariantList LayerManager::yGridAxisRight() const
{
    if (rootLayer()) {
        return rootLayer()->gridAxis("yaxisvaluesright");
    }

    return QVariantList();
}

void LayerManager::broadCast(const QVariantMap& parameters) {
    emit linkSendMessage(parameters);
}










