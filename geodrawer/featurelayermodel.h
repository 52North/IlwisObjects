#ifndef FEATURELAYERMODEL_H
#define FEATURELAYERMODEL_H

#include "coveragedisplay/oglbuffer.h"
#include "ilwiscoreui_global.h"

#include <QColor>
#include <QVariant>
#include <QVector3D>


namespace Ilwis {
class FeatureCoverage;
typedef IlwisData<FeatureCoverage> IFeatureCoverage;

namespace Ui {

class LayerManager;
class LayerModel;

struct VertexIndex {
    VertexIndex(quint32 start=0, quint32 count=0, quint32 oglType= 0x0003 /*GL_LINE_STRIP*/, Raw vid=iUNDEF) :
        _start(start),
        _count(count),
        _oglType(oglType),
        _objectid(vid){}

    quint32 _start;
    quint32 _count;
    quint32 _oglType;
    Raw _objectid = iUNDEF;
};

struct FeatureDrawing{
        FeatureDrawing(IlwisTypes ilwtype=itUNKNOWN) : _geomtype(ilwtype) {}
        IlwisTypes _geomtype;
        QVector3D _center;
		QVector<qreal> _vertices;
		QVector<qreal> _colors;
};

class FeatureLayerModel : public CoverageLayerModel
{
	friend class PointLayerModel;
	friend class LineLayerModel;
	friend class PolygonLayerModel;
	friend class VectorLayerModel;

public:
	enum ChildLayers{clPOINT=0, clLINE=1, clAREA=2, clBOUNDARY=4};

	FeatureLayerModel();
    FeatureLayerModel( LayerManager *manager, QStandardItem *parent, const QString &name, const QString &desc, const IOOptions& options);

    Q_INVOKABLE void linkMessage(const QVariantMap& parms);
    
    void coverage(const ICoverage& cov);
	ICoverage coverage() const;
    QVariant vproperty(const QString& attrName) const ;
    void vproperty(const QString& attrName, const QVariant& value);
    void fillAttributes() ;
    QString value2string(const QVariant &value, const QString &attrName) const;
	bool prepare(int);
    int numberOfBuffers(const QString&) const;
    QVariantList linkProperties() const;


    static LayerModel *create(LayerManager *, QStandardItem *,const QString&, const QString&, const IOOptions&);

    NEW_LAYERMODEL(FeatureLayerModel);

    public slots:
        void linkAcceptMessage(const QVariantMap& parameters);

private:
    IFeatureCoverage _features;
	std::vector<Ilwis::Ui::FeatureDrawing> _featureDrawings;
};
}
}

#endif // FEATURELAYERMODEL_H
