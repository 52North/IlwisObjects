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
    virtual bool renderReady();
    virtual void renderReady(bool yesno);

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
