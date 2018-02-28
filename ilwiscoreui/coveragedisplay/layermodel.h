#ifndef LAYERMODEL_H
#define LAYERMODEL_H

#include <QObject>
#include <QQmlListProperty>
#include <QVector>
#include <set>
#include "identity.h"
#include "tree.h"
#include "ilwiscoreui_global.h"

#define NEW_LAYERMODEL(name) \
    private: \
static int dummy_layermodelid_name;

#define NEW_LAYERMODEL2(name) static int dummy_layermodelid_name2;
#define NEW_LAYERMODEL3(name) static int dummy_layermodelid_name3;

#define REGISTER_LAYERMODEL(modelname, name) \
    int name::dummy_layermodelid_name = Ilwis::Ui::LayerManager::registerLayerModel(modelname, name::create);
#define REGISTER_LAYERMODEL2(modelname, name) \
    int name::dummy_layermodelid_name2 = Ilwis::Ui::LayerManager::registerLayerModel(modelname, name::create);
#define REGISTER_LAYERMODEL3(modelname, name) \
    int name::dummy_layermodelid_name3 = Ilwis::Ui::LayerManager::registerLayerModel(modelname, name::create);

namespace Ilwis {
class Coordinate;
class IOOptions;

namespace Ui {

typedef int LayerIndex;
class LayerInfoItem;
class VisualAttribute;
class LayerManager;


class ILWISCOREUISHARED_EXPORT LayerModel : public TreeNode, public Ilwis::Identity
{
    Q_OBJECT
public:
    enum ColorValueMeaning{cvmFRACTION, cvmTRUEVALUE};
	enum PreparationType { ptNONE = 0, ptRENDER = 1, ptGEOMETRY = 2, ptALL = 4294967295 };

	Q_ENUMS(PreparationType)
    Q_PROPERTY(bool active READ active WRITE active NOTIFY onActiveChanged)
    Q_PROPERTY(QQmlListProperty<Ilwis::Ui::VisualAttribute> visualAttributes READ vattributes NOTIFY visualAttributesChanged)
    Q_PROPERTY(QQmlListProperty<Ilwis::Ui::TreeNode> layers READ layersPrivate NOTIFY layerChanged)
    Q_PROPERTY(QString icon READ icon CONSTANT)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
	Q_PROPERTY(QString layerId READ layerId CONSTANT)
    Q_PROPERTY(bool isDrawable READ isDrawable CONSTANT)
    Q_PROPERTY(bool isVectorLayer READ isVectorLayer CONSTANT)
	Q_PROPERTY(bool usesColorData READ usesColorData CONSTANT)
	Q_PROPERTY(double opacity READ opacity WRITE opacity NOTIFY opacityChanged)
	Q_PROPERTY(QStringList changedProperties READ changedProperties CONSTANT)
	Q_PROPERTY(bool updateGeometry READ updateGeometry WRITE updateGeometry NOTIFY geometryChanged)
	Q_PROPERTY(int layerCount READ layerCount NOTIFY layerCountChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY validChanged)
	Q_PROPERTY(bool isSupportLayer READ isSupportLayer CONSTANT)
	Q_PROPERTY(QString drawType READ drawType CONSTANT)
    Q_PROPERTY(bool isCoverageBased READ isCoverageBased CONSTANT)

	Q_INVOKABLE virtual bool prepare(int);
	Q_INVOKABLE virtual int numberOfBuffers(const QString&) const;
	Q_INVOKABLE virtual QVector<qreal> vertices(qint32 bufferIndex, const QString& tp) const;
	Q_INVOKABLE virtual QVector<int> indices(qint32 bufferIndex, const QString& tp) const;
	Q_INVOKABLE virtual QVector<qreal> colors(qint32 bufferIndex, const QString& tp) const;
	Q_INVOKABLE void removeFromChangedProperties(const QString& prop);
	Q_INVOKABLE Ilwis::Ui::LayerModel *layer(int idx);
	Q_INVOKABLE virtual QVariant vproperty(const QString& key) const;
	Q_INVOKABLE virtual void vproperty(const QString& key, const QVariant& value);
	Q_INVOKABLE quint32 meshCount() const;
	Q_INVOKABLE quint32 meshIndex(int n) const;
	Q_INVOKABLE void addMeshIndex(quint32 idx);
	Q_INVOKABLE void clearMeshIndexes();
	Q_INVOKABLE bool isPrepared(quint32 type) const;
    Q_INVOKABLE virtual void setActiveAttribute(int idx);


	LayerModel();
    explicit LayerModel(TreeNode *parent);
    LayerModel(LayerManager *manager, QObject *parent, const QString &name, const QString &desc, const IOOptions& options);

    virtual void redraw() const;
    int activeVProperty() const;
    void activeVProperty(int index);
    QString visualVPropertyByIndex(int index) const;
    template<class T> T* as(){ return static_cast<T*>(this); }


    void addLayer(LayerModel *layer);
    LayerModel *parentLayer() const;
    void clearLayers();
    int layerCount() const;
	LayerModel *findLayerByName(const QString& name);
    const LayerModel * findLayerByName(const QString & name) const;
    void moveLayer(LayerModel *lyr, int type);
    virtual QString url() const;
	virtual void fillAttributes();
    void addVisualAttribute(VisualAttribute *attr);
    virtual QString layerData(const Coordinate &crdIn, const QString& attrName, QVariantList &items) const;
    virtual QString icon() const;
	virtual QString layerId() const;
	IlwisTypes layerType() const;
	QString drawType() const;
    double opacity() const;
    void opacity(double opacity);
    virtual bool active() const;
    virtual void active(bool yesno);
    QStringList changedProperties() const;
	void add2ChangedProperties(const QString& prop, bool propagate = false);
	bool updateGeometry() const;
	void updateGeometry(bool yesno);
    void updateGeometry(bool yesno, bool propagate);
	bool isValid() const;
	bool isDrawable() const;
	bool isVectorLayer() const;
	bool isSupportLayer() const;
	virtual bool usesColorData() const;
    void fillData();
    const LayerManager *layerManager() const;
    LayerManager *layerManager();
    virtual void activeAttributeName(const QString& pName);
    virtual VisualAttribute *activeAttribute();
    virtual VisualAttribute *visualAttribute(const QString &attrName) const;
    void order(qint32 n);
    qint32 order() const;
    virtual bool isCoverageBased() const;
    bool hasFixedStructure() const;


    static LayerModel *create(LayerManager *manager, LayerModel *layer, const QString &name, const QString &desc, const IOOptions& options);
protected:
    void isSupportLayer(bool yesno);

signals:
       void onActiveChanged();
       void visualAttributesChanged();
       void layerChanged();
       void nameChanged();
	   void verticesChanged();
	   void colorsChanged();
	   void indicesChanged();
	   void opacityChanged();
	   void geometryChanged();
	   void layerCountChanged();
	   void validChanged();
	   void prepareChanged();

public slots:

protected:
       QList<VisualAttribute *> _visualAttributes;
	   QString _icon = "folderFeature20.png";
	   bool _isDrawable = false;
	   bool _isVector = false;
	   int _prepared = 0;
	   IlwisTypes _layerType = itUNKNOWN;
	   bool _isValid = false;
       bool _hasFixedStructure = false;

private:
    LayerManager *_layerManager;
    bool _active = true;
    double _opacity = 1.0;
    LayerModel *_parent;
    int _activeVProperty = 0;
    static quint64 _layerId;
	std::vector<quint32> _meshes;
	bool _geometryChanged = false;
    qint32 _order = iUNDEF;

	std::set<QString> _changedProperties;
	bool _isSupportLayer = false;

    QQmlListProperty<VisualAttribute> vattributes();
    QQmlListProperty<TreeNode> layersPrivate();


    //NEW_LAYERMODEL(LayerModel)

};

const quint64 itROOTLAYER = itLAYER | itCOLLECTION;
const quint64 itCOMPOSITELAYER = itLAYER | itARRAY;
const quint64 itCOVERAGELAYER = itLAYER | itCOVERAGE;
const quint64 itRASTERLAYER = itLAYER | itRASTER;
const quint64 itPOINTLAYER = itLAYER | itPOINT;
const quint64 itLINELAYER = itLAYER | itLINE;
const quint64 itPOLYGONLAYER = itLAYER | itPOLYGON;
const quint64 itGRIDLAYER = itLINELAYER | itCONVENTIONALCOORDSYSTEM;
const quint64 itGRATTICULELAYER = itLINELAYER | itPROJECTION;
const quint64 itFEATURELAYER = itLINELAYER | itPOINTLAYER | itPOLYGONLAYER;

}
}

#endif // LAYERMODEL_H
