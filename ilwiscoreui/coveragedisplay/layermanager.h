#pragma once

#include <memory>
#include <QObject>
#include <QAbstractItemModel>
#include "kernel.h"
#include "ilwisdata.h"
#include "coverage.h"
#include "layermodel.h"
#include "rootlayermodel.h"
#include "ilwiscoreui_global.h"

class QQuickItem;

namespace Ilwis {
class GeoReference;
class CoordinateSystem;
typedef IlwisData<GeoReference> IGeoReference;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;


namespace Ui {

class UIContextModel;
class RootLayerModel;
class LayerModel;
class CoverageLayerModel;
class LayerInfoItem;
class LayerManager;
class IntermediateLayerModel;

typedef std::function<LayerModel *(LayerManager *,LayerModel *,const QString&, const QString&, const IOOptions&)> CreateLayer;

class ILWISCOREUISHARED_EXPORT LayerManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QAbstractItemModel * layerTree READ layerTree NOTIFY layerTreeChanged)
    Q_PROPERTY(QQmlListProperty<Ilwis::Ui::LayerModel> layerList READ layerList NOTIFY layerListChanged)
    Q_PROPERTY(quint32 viewid READ viewid CONSTANT)
    Q_PROPERTY(bool hasSelectionDrawer READ hasSelectionDrawer WRITE setHasSelectionDrawer NOTIFY hasSelectionDrawerChanged)
    Q_PROPERTY(bool zoomInMode READ zoomInMode WRITE setZoomInMode NOTIFY zoomInModeChanged)
    Q_PROPERTY(bool zoomOutMode READ zoomOutMode WRITE setZoomOutMode NOTIFY zoomOutModeChanged)
    Q_PROPERTY(bool panningMode READ panningMode WRITE setPanningMode NOTIFY panningModeChanged)
    Q_PROPERTY(Ilwis::Ui::RootLayerModel* rootLayer READ rootLayer CONSTANT)
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)
    Q_PROPERTY(int layerCount READ layerCount NOTIFY layerCountChanged)
    Q_PROPERTY(bool needUpdate READ needUpdate WRITE needUpdate NOTIFY needUpdateChanged)
    Q_PROPERTY(LayerModel *lastAddedCoverageLayer READ lastAddedCoverageLayer CONSTANT)

public:
	enum LayerMovement { lmUP, lmDOWN, lmREMOVE };

    explicit LayerManager();
    LayerManager(QObject *parent, QQuickItem *viewContainer);
    ~LayerManager();

    Q_INVOKABLE void setLayerListName(const QString name);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void addCommand(const QString& expression);
	Q_INVOKABLE void viewArea(QQuickItem *area);
	Q_INVOKABLE void wholeMap();
	Q_INVOKABLE  Ilwis::Ui::LayerModel* findLayer(int nodeid);

    RootLayerModel *rootLayer() const;
    
    bool hasSelectionDrawer() const;
    void setHasSelectionDrawer(bool yesno);
	bool zoomInMode() const;
	void setZoomInMode(bool yesno);
	bool zoomOutMode() const;
	void setZoomOutMode(bool yesno);
	bool panningMode() const;
	void setPanningMode(bool yesno);

    QString layerListName() const;
	TreeModel *layerTree();
	LayerModel *lastAddedCoverageLayer() const;
	void lastAddedCoverageLayer(LayerModel *lyr);
	QQmlListProperty<Ilwis::Ui::LayerModel> layerList() ;
	QList<Ilwis::Ui::LayerModel *> layerList2() ;
	QString layerData(const Coordinate &crdIn, const QString& attrName, QVariantList &items) const;


    static LayerModel *create(LayerModel *parentLayer,  const ICoverage& cov, LayerManager *lm, const IOOptions& options = IOOptions());

   

    static LayerModel *create(LayerModel *parentLayer,  const QString& type, LayerManager *lm, const QString& layername, const QString& description="", const IOOptions& options= IOOptions());
    static int registerLayerModel(const QString& modelname, CreateLayer);

    void moveLayer(LayerModel *parentLayer, LayerModel *layer, LayerMovement type);

    void clearLayers(LayerModel *parentLayer);
	int layerCount();
    quint32 viewid() const;
    bool isValid() const;
	QQuickItem *viewArea() const;

	void needUpdate(bool yesno);
	bool needUpdate() const;
    int nextId();

signals:
    void removeLayer(const Ilwis::Resource& resource);
    void layerListChanged();
	void layerTreeChanged();
    void hasSelectionDrawerChanged();
	void zoomInModeChanged();
    void zoomOutModeChanged();
	void panningModeChanged();
    void isValidChanged();
	void layerCountChanged();
	void needUpdateChanged();

private:
    RootLayerModel *_globalLayer = 0;
	IntermediateLayerModel *_dummyRootNode = 0; // this is a dummy root as every tree has to have a root but this root is not actually used it exist to prevent a visible root in the layer tree which serves no purpose
    TreeModel  *_tree = 0;
	QList<LayerModel *> _layerList;
    static std::map<QString, CreateLayer> _createLayers;
    bool _hasSelectionDrawer = false;
	bool _zoomInMode = false;
    bool _zoomOutMode = false;
	bool _panningMode = false;
    QString _layerListName = sUNDEF;
    quint32 _viewid = iUNDEF;
	QQuickItem *_viewContainer = 0;
	int _nodeCounter = 0;
    static quint32 _baseViewId;
	bool _needUpdate = false; // needed when a property of the whole rendering changed (e.g. zoom)
	LayerModel *_lastAddedCoverageLayer = 0;

    static void addLayer(LayerModel * parentLayer, LayerModel * layer, LayerManager * lm);



};

}
}

