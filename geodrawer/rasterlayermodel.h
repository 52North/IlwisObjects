#ifndef RASTERLAYERMODEL_H
#define RASTERLAYERMODEL_H
#include "ilwiscoreui_global.h"
#include "coveragedisplay\coveragelayermodel.h"
#include "location.h"
#include "geos/geom/Coordinate.h"
#include "ilwiscoordinate.h"
#include "box.h"

namespace Ilwis {
class RasterCoverage;
typedef IlwisData<RasterCoverage> IRasterCoverage;

namespace Ui {

class Quad {
public:
    Quad(const unsigned int imageOffsetX, const unsigned int imageOffsetY, const unsigned int imageSizeX, const unsigned int imageSizeY, const unsigned int zoomFactor);
    void addQuad(const Coordinate & c1, const Coordinate & c2, const Coordinate & c3, const Coordinate & c4, const double s1, const double t1, const double s2, const double t2);
    void setId(qint32 id);
    bool operator ==(const Ilwis::Ui::Quad &_quad);
    unsigned int imageOffsetX;
    unsigned int imageOffsetY;
    unsigned int imageSizeX;
    unsigned int imageSizeY;
    unsigned int zoomFactor;
    qint32 id;
	QVector<qreal> vertices;
    QVector<qreal> uvs;
	QVector<int> indices;
    bool active;
    bool refresh;
};

class TextureHeap;

class RasterLayerModel : public CoverageLayerModel
{
    Q_OBJECT
public:
	RasterLayerModel();
    RasterLayerModel(LayerManager *manager, QStandardItem *parent, const QString &name, const QString &desc, const IOOptions& options);
    ~RasterLayerModel();
    Q_PROPERTY(QVariantMap palette READ palette NOTIFY paletteChanged);
    Q_PROPERTY(QVector<qint32> removeQuads READ removeQuads)

	void coverage(const ICoverage& cov);
    QVariant vproperty(const QString& attrName) const ;
    void vproperty(const QString& attrName, const QVariant& value);
    void fillAttributes() ;
	bool prepare(int);
    QString value2string(const QVariant &value, const QString &attrName) const;
    QString layerData(const Coordinate &crdIn, const QString& attrName, QVariantList &items) const;
    static LayerModel *create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options);
	bool usesColorData() const;
    QVariantList linkProperties() const;
    Q_INVOKABLE virtual int numberOfBuffers(const QString&) const;
    Q_INVOKABLE virtual QVector<qreal> vertices(qint32 bufferIndex, const QString& ) const;
    Q_INVOKABLE virtual QVector<qreal> uvs(qint32 bufferIndex) const;
    Q_INVOKABLE virtual QVector<int> indices(qint32 bufferIndex, const QString& ) const;
    Q_INVOKABLE virtual QVariantMap texture(qint32 bufferIndex);
    Q_INVOKABLE void setQuadId(qint32 bufferIndex, qint32 id);

protected:
    TextureHeap * textureHeap;

private:
    void DivideImage(unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY);
    void GenerateQuad(Coordinate & c1, Coordinate & c2, Coordinate & c3, Coordinate & c4, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY, unsigned int zoomFactor);
    QVariantMap palette();
    void init();
    void refreshPalette();
    IRasterCoverage _raster;
    std::vector<Quad> _quads;
    std::vector<qint32> _addQuads;
    QVector<qint32> _removeQuads;
    QVector<qint32> removeQuads();
    int _maxTextureSize;
    int _paletteSize;
    QVariantMap _palette;
    unsigned long _imageWidth;
    unsigned long _imageHeight;
    unsigned long _width;
    unsigned long _height;
    bool _linear;
    bool _initDone;

public slots:
    void requestRedraw();

signals:
    void paletteChanged();

    NEW_LAYERMODEL(RasterLayerModel);
};
}
}

#endif // RASTERLAYERMODEL_H
